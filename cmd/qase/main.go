package main

import (
	"github.com/spf13/cobra"
)

func main() {
	var rootCmd = &cobra.Command{Use: "qase"}
	var playCmd = &cobra.Command{
		Use:   "play",
		Short: "Play music",
		Run: func(cmd *cobra.Command, args []string) {
			start_music(args[0])
		},
	}
	rootCmd.AddCommand(playCmd)
	rootCmd.Execute()

}
