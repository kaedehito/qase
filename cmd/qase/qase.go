package main

import (
	"log"
	"os"
	"sync"
	"time"

	"github.com/dhowden/tag"
	"github.com/faiface/beep"
	"github.com/faiface/beep/effects"
	"github.com/faiface/beep/flac"
	"github.com/faiface/beep/mp3"
	"github.com/faiface/beep/speaker"
	"github.com/faiface/beep/vorbis"
	"github.com/faiface/beep/wav"
)

var (
	stop       bool
	quit       bool
	musicName  string
	total_time time.Duration
	mu         sync.Mutex
)

func start_music(filePath string) {
	file, err := os.Open(filePath)
	if err != nil {
		log.Fatal(err)
		os.Exit(1)
	}

	metadata, err := tag.ReadFrom(file)
	if err != nil {
		musicName = file.Name()
	} else {
		musicName = metadata.Title()
	}

	defer file.Close()

	var streamer beep.StreamSeekCloser
	var format beep.Format

	if is_mp3(filePath) {
		streamer, format, err = mp3.Decode(file)
	} else if is_wav(filePath) {
		streamer, format, err = wav.Decode(file)
	} else if is_flac(filePath) {
		streamer, format, err = flac.Decode(file)
	} else if is_vorbis(filePath) {
		streamer, format, err = vorbis.Decode(file)
	} else {
		log.Fatal("Unsupported file format")
	}

	if err != nil {
		log.Fatal(err)
		os.Exit(1)
	}

	play_music(streamer, format)
}

var (
	volume      *effects.Volume
	loopEnabled bool
)

func play_music(streamer beep.StreamSeekCloser, format beep.Format) {

	speaker.Init(format.SampleRate, format.SampleRate.N(time.Second/10))

	ctrl := &beep.Ctrl{Streamer: beep.Loop(1, streamer), Paused: false}
	volume = &effects.Volume{Streamer: ctrl, Base: 2, Volume: 0}
	speaker.Play(volume)

	mu.Lock()
	total_time = time.Duration(streamer.Len()) * time.Second / time.Duration(format.SampleRate)
	mu.Unlock()

	go start_cli()

	for {
		if loopEnabled {
			beep.Loop(-1, streamer)
		} else {
			beep.Loop(1, streamer)
		}

		if quit {
			os.Exit(0)
		}

		if stop {
			ctrl.Paused = true
		} else {
			ctrl.Paused = false
		}

		if streamer.Err() == nil && streamer.Position() >= streamer.Len() {
			os.Exit(0)
		}

	}
}
