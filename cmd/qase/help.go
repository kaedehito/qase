package main

import (
	"github.com/rivo/tview"
)

func help() {
	app := tview.NewApplication()
	flex := tview.NewFlex().SetDirection(tview.FlexRow).
		AddItem(tview.NewBox().SetBorder(true).SetTitle("Help").SetBorderPadding(1, 1, 2, 2), 0, 1, false).
		AddItem(tview.NewBox().SetBorder(true).SetTitle("Commands").SetBorderPadding(1, 1, 2, 2), 0, 1, false).
		AddItem(tview.NewBox().SetBorder(true).SetTitle("Options").SetBorderPadding(1, 1, 2, 2), 0, 1, false).
		AddItem(tview.NewBox().SetBorder(true).SetTitle("Examples").SetBorderPadding(1, 1, 2, 2), 0, 1, false).
		AddItem(tview.NewBox().SetBorder(true).SetTitle("Exit").SetBorderPadding(1, 1, 2, 2), 0, 1, false)

	if err := app.SetRoot(flex, true).Run(); err != nil {
		panic(err)
	}

	
}

func page1() {
	
}
