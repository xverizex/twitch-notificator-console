#include <AL/al.h>
#include <AL/alc.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>

static ALCdevice *device;
static ALCcontext *ctx;
static uint32_t source;
static int is_alert = 0;
static uint32_t buf;

void sound_init (const char *filepath) {
	if (access (filepath, F_OK)) {
		perror ("sound_init");
		return;
	}

	device = alcOpenDevice (NULL);
	ctx = alcCreateContext (device, NULL);
	alcMakeContextCurrent (ctx);

	alGenBuffers (1, &buf);

	/* read file */
	FILE *fp = fopen (filepath, "r");
	fseek (fp, 0, SEEK_END);
	size_t len_file = ftell (fp);
	fseek (fp, 0, SEEK_SET);
	char *data = malloc (len_file);
	fread (data, 1, len_file, fp);
	fclose (fp);

	alBufferData (buf, AL_FORMAT_MONO16, data, len_file, 44100);

	alGenSources (1, &source);
	alSourcei (source, AL_BUFFER, buf);

	is_alert = 1;
}

void sound_play () {
	if (is_alert) alSourcePlay (source);
}
