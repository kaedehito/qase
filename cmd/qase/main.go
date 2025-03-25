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
	var versionCmd = &cobra.Command{
		Use:   "version",
		Short: "Print the version number of qase",
		Run: func(cmd *cobra.Command, args []string) {
			println("qase 4.0 strawberry")
			println("Copyright (c) 2024 futo ogasawara <info@pik6c.tech>")
			println("This software is licensed under the Qase License")
		},
	}
	rootCmd.AddCommand(versionCmd)
	rootCmd.AddCommand(playCmd)
	rootCmd.Execute()

}
