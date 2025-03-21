package main

import (
	"log"
	"os"

	"github.com/faiface/beep/mp3"
)

func is_mp3(uri string) bool {
	file, err := os.Open(uri)
	if err != nil {
		log.Fatal(err)
		os.Exit(1)
	}
	defer file.Close()
	_, _, err = mp3.Decode(file)
	if err != nil {
		return false
	}
	return true
}
