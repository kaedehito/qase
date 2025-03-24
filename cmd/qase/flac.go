package main

import (
	"log"
	"os"

	"github.com/faiface/beep/flac"
)

func is_flac(path string) bool {
	file, err := os.Open(path)
	if err != nil {
		log.Fatal(err)
		os.Exit(1)
	}
	defer file.Close()
	_, _, err = flac.Decode(file)
	return err == nil
}
