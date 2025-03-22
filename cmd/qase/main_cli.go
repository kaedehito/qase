package main

import (
	tea "github.com/charmbracelet/bubbletea"
)

type model struct {
	now_time currentTime
	message  string
	errormsg string
	volume   int
	stop     bool
	loop     bool
	help     bool
}

// Init implements tea.Model.
func (m model) Init() tea.Cmd {
	return nil
}

// Update implements tea.Model.
func (m model) Update(msg tea.Msg) (tea.Model, tea.Cmd) {
	switch msg := msg.(type) {
	case tea.KeyMsg:
		switch msg.String() {
		case "q":
			return m, tea.Quit
		case "h":
			m.help = !m.help
			return m, nil
		case "s":
			if m.stop {
				m.errormsg = "Music is already stopped!"
				return m, nil
			}
			m.stop = !m.stop
			return m, nil
		case "p":
			if m.stop {
				m.stop = false
				m.message = "Play"
			} else {
				m.errormsg = "Music is already playing!"
			}
		case "l":
			m.loop = !m.loop
			return m, nil
		case "j":
			m.volume -= 1
			if m.volume < 0 {
				m.errormsg = "Volume is already at minimum!"
				m.volume = 0
			}
			return m, nil
		case "k":
			m.volume += 1
			if m.volume > 100 {
				m.errormsg = "Volume is already at maximum!"
				m.volume = 100
			}
			return m, nil
		}
	}
	return m, nil
}

// View implements tea.Model.
func (m model) View() string {
	if m.help {
		help()
	}

	if m.stop {
		mu.Lock()
		stop = true
		mu.Unlock()
		return "Music stopped"
	} else {
		stop = false
	}

	if m.loop {
		mu.Lock()
		loop = true
		mu.Unlock()
		return "Music loop is enabled"
	}

	if m.message != "" {
		return m.message
	}

	if m.errormsg != "" {
		return "\x1b[31m" + m.errormsg + "\x1b[0m"
	}

	if m.stop {
		return ""
	}

}

type currentTime struct {
	hour   int
	minute int
	second int
}

var _ tea.Model = model{
	now_time: currentTime{
		hour:   0,
		minute: 0,
		second: 0,
	},
	message: "",
	volume:  0,
	stop:    false,
	loop:    false,
	help:    false,
}

func start_cli() {
	m := model{
		now_time: currentTime{
			hour:   0,
			minute: 0,
			second: 0,
		},
		message: "",
		volume:  0,
		stop:    false,
		loop:    false,
		help:    false,
	}

	p := tea.NewProgram(m)
	p.Run()

}
