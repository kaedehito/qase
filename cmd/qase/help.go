package main

import (
	"github.com/gdamore/tcell/v2"
	"github.com/rivo/tview"
)

func help() {
	app := tview.NewApplication()

	information := tview.NewTextView().
		SetText(" ### Welcome to qase 4.0(strawberry)! ###").
		SetTextColor(tcell.ColorWhite).
		SetTitle(" INFOMATION ").
		SetTitleAlign(tview.AlignLeft).
		SetBorder(true).
		SetBackgroundColor(tcell.ColorDefault)

	app.SetFocus(information)

	// Enable mouse support
	app.EnableMouse(true)

	if err := app.SetRoot(information, true).Run(); err != nil {
		panic(err)
	}
}
