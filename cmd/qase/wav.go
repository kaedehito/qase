package main

import (
	"log"
	"os"

	"github.com/faiface/beep/wav"
)

func is_wav(uri string) bool {
	file, err := os.Open(uri)
	if err != nil {
		log.Fatal(err)
		os.Exit(1)
	}
	defer file.Close()
	_, _, err = wav.Decode(file)
	if err != nil {
		return false
	}
	return true
}
