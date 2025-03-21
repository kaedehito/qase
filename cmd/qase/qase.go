package main

import (
	"log"
	"os"
	"time"

	"github.com/faiface/beep"
	"github.com/faiface/beep/effects"
	"github.com/faiface/beep/mp3"
	"github.com/faiface/beep/speaker"
	"github.com/faiface/beep/wav"
)

func start_music(uri string) {
	file, err := os.Open(uri)
	if err != nil {
		log.Fatal(err)
		os.Exit(1)
	}
	defer file.Close()

	if is_mp3(uri) {
		streamer, format, _ := mp3.Decode(file)
		play_music(streamer, format)
	} else if is_wav(uri) {
		streamer, format, _ := wav.Decode(file)
		play_music(streamer, format)
	} else {
		log.Fatal("Unsupported file format")
	}

}

func play_music(streamer beep.StreamSeekCloser, format beep.Format) {

	speaker.Init(format.SampleRate, format.SampleRate.N(time.Second/10))

	ctrl := &beep.Ctrl{Streamer: beep.Loop(-1, streamer), Paused: false}
	volume := &effects.Volume{Streamer: ctrl, Base: 2}
	speaker.Play(volume)
	select {}
}
