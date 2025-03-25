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
	message2 string
	volume   int
	stop     bool
	loop     bool
}

// Init implements tea.Model.
func (m model) Init() tea.Cmd {
	return tickCmd()
}

func tickCmd() tea.Cmd {
	return tea.Tick(time.Second, func(t time.Time) tea.Msg {
		return tickMsg{}
	})
}

type tickMsg struct{}

var count int
var isError bool

// Update implements tea.Model.
func (m model) Update(msg tea.Msg) (tea.Model, tea.Cmd) {
	m.message2 = fmt.Sprintf("\x1b[35mNow Playing %s\x1b[0m", musicName)
	if count > 30 {
		if isError {
			m.message2 = ""
			isError = false
		}
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
		case "s":
			if m.stop {
				m.message2 = "\x1b[31mMusic is already stopped!\x1b[0m"
				isError = true
				return m, nil
			}
			m.stop = !m.stop
			return m, nil
		case "p":
			if m.stop {
				m.stop = false
				m.message = "Play"
			} else {
				m.message2 = "\x1b[31mMusic is already playing!\x1b[0m"
				isError = true
			}
		case "l":
			m.loop = !m.loop
			return m, nil
		case "j":
			m.volume -= 1
			if m.volume < 0 {
				m.message2 = "\x1b[31mVolume is already at minimum!\x1b[0m"
				isError = true
				m.volume = 0
				return m, nil
			}
			volume.Volume -= 0.1
			m.message = fmt.Sprintf("Volume: %d", m.volume)
			return m, nil
		case "k":
			m.volume += 1
			if m.volume > 100 {
				m.message2 = "\x1b[31mVolume is already at maximum!\x1b[0m"
				isError = true
				m.volume = 100
				return m, nil
			}
			volume.Volume += 0.1
			m.message = fmt.Sprintf("Volume: %d", m.volume)
			return m, nil
		}
	case tickMsg:
		if !m.stop {
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

		return m, tickCmd()
	}

	return m, nil
}

// View implements tea.Model.
func (m model) View() string {

	mu.Lock()
	stop = m.stop
	loopEnabled = m.loop
	mu.Unlock()

	var symbol string
	if stop {
		symbol = "⏸"
	} else {
		symbol = "▶"
	}
	if loopEnabled {
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

	if m.message2 != "" {
		message = fmt.Sprintf("%s\n%s", m.message2, message)
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
		volume:  50,
		stop:    false,
		loop:    false,
	}

	mu.Lock()
	m.now_time.total = int(total_time.Seconds())
	mu.Unlock()

	p := tea.NewProgram(m)
	p.Run()
}
