package main

import (
	"log"
	"os"

	"github.com/faiface/beep/vorbis"
)

func is_vorbis(path string) bool {
	file, err := os.Open(path)

	if err != nil {
		log.Fatal(file)
		os.Exit(1)
	}

	defer file.Close()

	_, _, err = vorbis.Decode(file)
	return err == nil
}
