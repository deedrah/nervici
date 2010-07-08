#include <al.h>
#include <alc.h>
#include <SDL.h>
#include <dirent.h>
//#include <string.h>
#include <vorbisfile.h>
#include <AL/al.h>
#include <vector>
#include <iostream>

#include "system.h"
#include "settings/setting.h"

#include "audio.h"

static const char* const wavs[] = {
    "/hop.wav", "/jau1.wav", "/jau2.wav", "/self.wav", "/smileplus.wav",
    "/smileminus.wav", "/wall1.wav", "/wall2.wav"
};

static const char* const section = "audio";
static const char* const st_sound = "sound";
static const char* const st_music = "music";
static const char* const st_buffer = "buffer";
static const char* const st_bfrCount = "bfrCount";

float Audio::music_rate = 1.0;
bool Audio::music_loop = false;
bool Audio::music_opened = false;
OggVorbis_File Audio::music_file;
vorbis_info *Audio::music_info;
ALuint *Audio::music_buffers;
ALuint Audio::music_source;
ALenum Audio::music_format;
MusicType Audio::music_type = MT_Count;
char *Audio::music_loader = NULL;
ALCdevice* Audio::device;
ALCcontext* Audio::context;
AudioSetting Audio::setting;
vector<SoundProfile> Audio::sound_profiles;
vector<PlAudio> Audio::sounds;
vector<MusicFile> Audio::music[MT_Count];

static ALfloat source_pos[] = {0.0, 0.0, 0.0};
static ALfloat source_vel[] = {0.0, 0.0, 0.0};

static ALfloat listener_pos[] = {0.0, 0.0, 0.0};
static ALfloat listener_vel[] = {0.0, 0.0, 0.0};
static ALfloat listener_ori[] = {0.0, 0.0, -1.0, 0.0, 1.0, 0.0};

static void scan_sounds_dir (const string& path, vector<SoundProfile>& profiles) {
    int w;
    int valid;
    DIR *dir;
    struct dirent *ent;
    FILE *file;
    string prof_path;
    string wav_path;
    SoundProfile entry;

    dir = opendir (path.c_str ());
    if (dir == NULL) return;

    for (ent = readdir (dir); ent != NULL; ent = readdir (dir)) {
        if (ent->d_name[0] == '.') continue;

        prof_path = path + '/' + ent->d_name;

        valid = 1;
        for (w = 0; w < WAVS_COUNT; w++) {
            wav_path = prof_path + wavs[w];

            file = fopen (wav_path.c_str (), "rb");
            if (file == NULL) {
                valid = 0;
                break;
            }
            fclose (file);
        }

        if (valid) {
            entry.directory = prof_path;
            entry.name = ent->d_name;
            profiles.push_back (entry);
        }
    }

    closedir (dir);
}

static ALuint load_wav (const string& filename) {
    SDL_AudioSpec spec;
    Uint8 *data;
    Uint32 len;
    ALuint result;

    alGenBuffers (1, &result);

    if (SDL_LoadWAV (filename.c_str (), &spec, &data, &len) == NULL) return result;
    switch (spec.channels) {
        case 1:
            switch (spec.format) {
                case AUDIO_S8:
                    alBufferData (result, AL_FORMAT_MONO8, data, len, spec.freq);
                    break;
                case AUDIO_S16LSB:
                case AUDIO_S16MSB:
                    alBufferData (result, AL_FORMAT_MONO16, data, len, spec.freq);
                    break;
            }
            break;
        case 2:
            switch (spec.format) {
                case AUDIO_S8:
                    alBufferData (result, AL_FORMAT_STEREO8, data, len, spec.freq);
                    break;
                case AUDIO_S16LSB:
                case AUDIO_S16MSB:
                    alBufferData (result, AL_FORMAT_STEREO16, data, len, spec.freq);
                    break;
            }
            break;
        default:
            break;
    }

    SDL_FreeWAV (data);

    return result;
}

static void load_buffers (vector<SoundProfile>& profiles) {
    int w;
    string filename;

    for (size_t pi = 0; pi < profiles.size (); pi++) {
        SoundProfile& spi = profiles[pi];

        for (w = 0; w < WAVS_COUNT; w++) {
            filename = spi.directory + wavs[w];
            spi.buffers[w] = load_wav (filename);
        }
    }
}

static double ogg_get_length (const string& filename) {
    OggVorbis_File file;
    double result;
    size_t len;
    char *fn;

    len = filename.length ();
    fn = (char *) alloca (len + 1);
    memcpy (fn, filename.c_str (), len + 1);

    if (ov_fopen (fn, &file)) return -1;

    result = ov_time_total (&file, -1);
    ov_clear (&file);

    return result;
}

static void scan_music_dir (const string& path, vector<MusicFile>& files, MusicType type) {
    DIR *dir;
    struct dirent *ent;
    double len;
    string file_path;
    MusicFile entry;

    dir = opendir (path.c_str ());
    if (dir == NULL) return;

    for (ent = readdir (dir); ent != NULL; ent = readdir (dir)) {
        if (ent->d_name[0] == '.') continue;

        file_path = path + '/' + ent->d_name;

        len = ogg_get_length (file_path);

        if (len <= 0) continue;
        if (type == MT_Short && len > 150) {
            continue;
        }
        if (type == MT_Long && len <= 150) {
            continue;
        }

        entry.filename = file_path;
        entry.played = false;
        files.push_back (entry);
    }

    closedir (dir);
}


void Audio::init_wavs () {
    cout << __func__ << '\n';

    scan_sounds_dir (System::get_sounds_dir (), sound_profiles);
    scan_sounds_dir (System::get_sounds_dir_home (), sound_profiles);

    load_buffers (sound_profiles);
}

void Audio::free_wavs () {
    for (size_t pi = 0; pi < sound_profiles.size (); pi++) {
        SoundProfile& spi = sound_profiles[pi];
        alDeleteBuffers (WAVS_COUNT, spi.buffers);
    }
}

void Audio::init_music () {
    static const char* const suffixs[MT_Count] = {"/game", "/game", "/menu", "/stat"};
    string dir_name;
    MusicType mt;

    cout << __func__ << '\n';

    for (mt = MT_Short; mt < MT_Count; mt++) {
        dir_name = System::get_music_dir () + suffixs[mt];
        scan_music_dir (dir_name, music[mt], mt);
    }

    for (mt = MT_Short; mt < MT_Count; mt++) {
        dir_name = System::get_music_dir_home () + suffixs[mt];
        scan_music_dir (dir_name, music[mt], mt);
    }

    music_loader = new char[setting.buffer];
    music_buffers = new ALuint[setting.bfrs_count];

    alGenBuffers (setting.bfrs_count, music_buffers);
    alGenSources (1, &music_source);

    alSourcefv (music_source, AL_POSITION, source_pos);
    alSourcefv (music_source, AL_VELOCITY, source_vel);
    alSourcef (music_source, AL_GAIN, setting.music / 20.0);
}

void Audio::free_music () {
    alDeleteBuffers (setting.bfrs_count, music_buffers);
    alDeleteSources (1, &music_source);

    delete [] music_loader;
    delete [] music_buffers;
}

void Audio::load_setting () {
    cout << __func__ << '\n';

    setting.sound = Setting::read_int (section, st_sound, 20);
    setting.music = Setting::read_int (section, st_music, 20);
    setting.buffer = Setting::read_int (section, st_buffer, 0x10000);
    setting.bfrs_count = Setting::read_int (section, st_bfrCount, 2);
}

void Audio::save_setting () {
    Setting::write_int (section, st_sound, setting.sound);
    Setting::write_int (section, st_music, setting.music);
    Setting::write_int (section, st_buffer, setting.buffer);
    Setting::write_int (section, st_bfrCount, setting.bfrs_count);
}

void Audio::initialize () {
    cout << __func__ << '\n';
    device = alcOpenDevice (NULL);
    context = alcCreateContext (device, NULL);
    alcMakeContextCurrent (context);

    alListenerfv (AL_POSITION, listener_pos);
    alListenerfv (AL_VELOCITY, listener_vel);
    alListenerfv (AL_ORIENTATION, listener_ori);

    load_setting ();

    init_wavs ();
    init_music ();
}

void Audio::uninitialize () {
    save_setting ();

    free_music ();
    free_wavs ();

    alcMakeContextCurrent (NULL);
    alcDestroyContext (context);
    alcCloseDevice (device);
}

int Audio::find_profil (const string& name) {
    for (size_t si = 0; si < sound_profiles.size (); si++) {
        if (strcasecmp (name.c_str (), sound_profiles[si].name.c_str ()) == 0) {
            return si;
        }
    }
    return -1;
}

void Audio::load_players (const GameInfo& info) {
    sounds.clear ();

    for (size_t si = 0; si < info.pl_infos.size (); si++) {
        PlAudio entry;
        alGenSources (1, &entry.source);
        alSourcef (entry.source, AL_PITCH, (info.pl_infos[si].pitch + 5) / 10.0);
        alSourcef (entry.source, AL_GAIN, setting.sound / 20.0);
        alSourcefv (entry.source, AL_VELOCITY, source_vel);
        alSourcefv (entry.source, AL_POSITION, source_pos);
        entry.prof = find_profil (info.pl_infos[si].profil);
        sounds.push_back (entry);
    }
}

void Audio::free_players () {
    for (size_t si = 0; si < sounds.size (); si++) {
        alSourceStop (sounds[si].source);
        alDeleteSources (1, &sounds[si].source);
    }
}

void Audio::play_effect (plid_tu plid, EffectType effect) {
    ALint play;
    //SoundProfile *spi;
    ALuint source;

    if (sounds[plid].prof >= 0) {
        alGetSourcei (sounds[plid].source, AL_SOURCE_STATE, &play);

        SoundProfile& spi = sound_profiles[sounds[plid].prof];
        source = sounds[plid].source;

        if (play != AL_PLAYING) {
            switch (effect) {
                case ET_Hop:
                    alSourcei (source, AL_BUFFER, spi.buffers[0]);
                    break;
                case ET_Jau:
                    alSourcei (source, AL_BUFFER, spi.buffers[1 + (sounds[plid].r % 2)]);
                    sounds[plid].r++;
                    break;
                case ET_Self:
                    alSourcei (source, AL_BUFFER, spi.buffers[3]);
                    break;
                case ET_SmilePlus:
                    alSourcei (source, AL_BUFFER, spi.buffers[4]);
                    break;
                case ET_SmileMinus:
                    alSourcei (source, AL_BUFFER, spi.buffers[5]);
                    break;
                case ET_Wall:
                    alSourcei (source, AL_BUFFER, spi.buffers[6 + (sounds[plid].r % 2)]);
                    sounds[plid].r++;
                    break;
            }

            alSourcePlay (source);
        }
    }
}

bool Audio::music_open (MusicType type) {
    int f;
    int op;
    size_t count;

    count = 0;
    for (size_t mi = 0; mi < music[type].size (); mi++) {
        count += music[type][mi].played;
    }

    if (count == music[type].size ()) {
        for (size_t mi = 0; mi < music[type].size (); mi++) {
            music[type][mi].played = false;
        }
        count = 0;
    }

    while (count < music[type].size ()) {
        cout << "music open " << count << '/' << music[type].size () << '\n';
        f = rand () % music[type].size ();
        if (!music[type][f].played) {
            music[type][f].played = true;
            count++;

            op = ov_fopen ((char *) music[type][f].filename.c_str (), &music_file);
            if (op) continue;

            music_info = ov_info (&music_file, -1);

            if (music_info->channels == 1)
                music_format = AL_FORMAT_MONO16;
            else if (music_info->channels == 2)
                music_format = AL_FORMAT_STEREO16;
            else {
                ov_clear (&music_file);
                continue;
            }

            music_opened = true;
            return true;
        }
    }

    return false;
}

void Audio::music_close () {
    if (music_opened) {
        ov_clear (&music_file);
        music_opened = false;
    }
}

int Audio::music_stream (ALuint buffer) {
    int size = 0;
    int section;
    int result;
    int bs;

    bs = setting.buffer / 2;
    while (bs - 1024 >= (setting.buffer / 2) * music_rate) bs -= 1024;
    while (bs < (setting.buffer / 2) * music_rate) bs += 1024;

    while (size < bs) {
        result = ov_read (&music_file, music_loader + size, bs - size, 0, 2, 1, &section);

        if (result > 0) size += result;
        else return result;
    }

    alBufferData (buffer, music_format, music_loader, size, music_info->rate * music_rate);
    return size;
}

void Audio::music_play (MusicType type) {
    ALenum state;
    int b;

    alGetSourcei (music_source, AL_SOURCE_STATE, &state);
    if (state == AL_PLAYING) {
        if (music_type == type) return;
    }

    music_stop ();

    music_type = type;
    if (!music_open (type)) return;

    for (b = 0; b < setting.bfrs_count; b++) {
        if (music_stream (music_buffers[b]) <= 0) {
            break;
        }
        alSourceQueueBuffers (music_source, 1, &music_buffers[b]);
    }

    alSourcePlay (music_source);
    music_loop = true;
}

void Audio::music_stop () {
    int queued;
    ALuint buffer;

    alSourceStop (music_source);
    alGetSourcei (music_source, AL_BUFFERS_QUEUED, &queued);

    for (; queued > 0; queued--) {
        alSourceUnqueueBuffers (music_source, 1, &buffer);
    }

    music_close ();
    music_loop = false;
}

void Audio::music_update () {
    int processed;
    ALenum state;
    ALuint buffer;

    alGetSourcei (music_source, AL_SOURCE_STATE, &state);
    if (state == AL_PLAYING) {

        alGetSourcei (music_source, AL_BUFFERS_PROCESSED, &processed);

        for (; processed > 0; processed--) {
            alSourceUnqueueBuffers (music_source, 1, &buffer);
            if (music_stream (buffer) > 0)
                alSourceQueueBuffers (music_source, 1, &buffer);
        }
    } else if (music_loop) {
        Audio::music_play (music_type);
    }

}

