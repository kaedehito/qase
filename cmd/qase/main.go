package main

import (
	"fmt"
	"github.com/spf13/cobra"
	"os"
)

func main() {
	var rootCmd = &cobra.Command{Use: "qase"}
	var playCmd = &cobra.Command{
		Use:   "play",
		Short: "Play music",
		Run: func(cmd *cobra.Command, args []string) {
			if len(args) > 0 {
				start_music(args[0])
			} else {
				fmt.Println("Error: No file specified")
			}
		},
	}
	var versionCmd = &cobra.Command{
		Use:   "version",
		Short: "Print the version number of qase",
		Run: func(cmd *cobra.Command, args []string) {
			fmt.Println("qase 4.0 strawberry")
			fmt.Println("Copyright (c) 2024 futo ogasawara <info@pik6c.tech>")
			fmt.Println("This software is licensed under the Qase License")
		},
	}

	rootCmd.AddCommand(versionCmd)
	rootCmd.AddCommand(playCmd)

	if err := rootCmd.Execute(); err != nil {
		fmt.Println("Error:", err)
		os.Exit(1)
	}
}
