package main

import (
	"fmt"
	"strings"
	"time"

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
	m.now_time.second += 1
	if m.now_time.second == 60 {
		m.now_time.second = 0
		m.now_time.minute += 1
	}
	if m.now_time.minute == 60 {
		m.now_time.minute = 0
		m.now_time.hour += 1
	}

	switch msg := msg.(type) {
	case tea.KeyMsg:
		switch msg.String() {
		case "q":
			mu.Lock()
			quit = true
			mu.Unlock()
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

	time.Sleep(1 * time.Second)
	return m, nil
}

// View implements tea.Model.
func (m model) View() string {
	if m.help {
		help()
	}

	mu.Lock()
	stop = m.stop
	loop = m.loop
	mu.Unlock()

	var symbol string
	if stop {
		symbol = "⏸"
	} else {
		symbol = "▶"
	}
	if loop {
		symbol = "↪ " + symbol
	}

	mu.Lock()
	totalSeconds := int(total_time.Seconds())
	mu.Unlock()

	hours := totalSeconds / 3600
	minutes := (totalSeconds % 3600) / 60
	seconds := totalSeconds % 60

	message := fmt.Sprintf("%s %d hour %d minutes %d seconds / %d hour %d minutes %d seconds\n", symbol, m.now_time.hour, m.now_time.minute, m.now_time.second, hours, minutes, seconds)

	if m.errormsg != "" {
		message = fmt.Sprintf("\x1b[31m%s\x1b[0m %s", m.errormsg, message)
	}

	if m.message != "" {
		message = m.message
	}

	replacements := map[string]string{
		"{symbol}": symbol,
		"{minute}": fmt.Sprintf("%d", m.now_time.minute),
		"{second}": fmt.Sprintf("%d", m.now_time.second),
	}
	for key, value := range replacements {
		message = strings.ReplaceAll(message, key, value)
	}

	return message
}

type currentTime struct {
	hour   int
	minute int
	second int
	total  int
}

var _ tea.Model = model{
	now_time: currentTime{
		hour:   0,
		minute: 0,
		second: 0,
		total:  0,
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
			total:  0,
		},
		message: "",
		volume:  0,
		stop:    false,
		loop:    false,
		help:    false,
	}

	mu.Lock()
	m.now_time.total = int(total_time.Seconds())
	mu.Unlock()

	p := tea.NewProgram(m)
	p.Run()
}
