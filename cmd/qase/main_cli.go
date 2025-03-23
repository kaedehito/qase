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
	go func() {
		for {
			time.Sleep(1 * time.Second)
			m.now_time.second += 1
			if m.now_time.second == 60 {
				m.now_time.minute += 1
				m.now_time.second = 0
			}
			if m.now_time.minute == 60 {
				m.now_time.hour += 1
				m.now_time.minute = 0
			}
		}
	}()
	return nil
}

var count int

// Update implements tea.Model.
func (m model) Update(msg tea.Msg) (tea.Model, tea.Cmd) {
	if count > 30 {
		m.errormsg = ""
		count = 0
	}
	count += 1

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
				return m, nil
			}
			m.message = fmt.Sprintf("Volume: %d", m.volume)
			return m, nil
		case "k":
			m.volume += 1
			if m.volume > 100 {
				m.errormsg = "Volume is already at maximum!"
				m.volume = 100
				return m, nil
			}
			m.message = fmt.Sprintf("Volume: %d", m.volume)
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

	var message string
	if m.now_time.hour == 0 {
		if m.message != "" {
			message = fmt.Sprintf(
				"%s\n%s %d minutes %d seconds / %d minutes %d seconds\n",
				m.message,
				symbol,
				m.now_time.minute,
				m.now_time.second,
				minutes,
				seconds,
			)

		} else {
			message = fmt.Sprintf(
				"%s %d minutes %d seconds / %d minutes %d seconds\n",
				symbol,
				m.now_time.minute,
				m.now_time.second,
				minutes,
				seconds,
			)
		}
	} else {
		if m.message != "" {
			message = fmt.Sprintf(
				"%s\n%s %d hour %d minutes %d seconds / %d hour %d minutes %d seconds\n",
				m.message,
				symbol,
				m.now_time.hour,
				m.now_time.minute,
				m.now_time.second,
				hours,
				minutes,
				seconds,
			)
		} else {
			message = fmt.Sprintf(
				"%s %d hour %d minutes %d seconds / %d hour %d minutes %d seconds\n",
				symbol,
				m.now_time.hour,
				m.now_time.minute,
				m.now_time.second,
				hours,
				minutes,
				seconds,
			)
		}
	}

	if m.errormsg != "" {
		message = fmt.Sprintf("\x1b[31m%s\x1b[0m\n%s", m.errormsg, message)
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

func (current currentTime) update_time(s chan currentTime) currentTime {
	for {
		time.Sleep(1 * time.Second)
		current.second += 1
		if current.second == 60 {
			current.minute += 1
			current.second = 0
		}
		if current.minute == 60 {
			current.minute = 0
			current.hour += 1
		}

	}

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
