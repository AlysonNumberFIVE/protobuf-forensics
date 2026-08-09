package main

import (
	"encoding/json"
	"fmt"
	"os"
)

func readStream(filename string) ([]byte, error) {
	data, err := os.ReadFile(filename)
	if err != nil {
		return nil, err
	}

	return data, nil
}

func printEvent(ev HolodeckEvent) {
	fmt.Println("timestamp:", ev.Timestamp)
	if ev.Event != "" {
		var parsed interface{}
		if err := json.Unmarshal([]byte(ev.Event), &parsed); err != nil {
			fmt.Println("  bad event JSON:", err)
			return
		}
		pretty, _ := json.MarshalIndent(parsed, "  ", "  ")
		fmt.Println(string(pretty))
	} else {
		fmt.Printf("  update: % x\n", ev.Update)
	}
}

func main() {
	content, err := readStream("../c5197a1e-30f4-4dfa-b98f-79a5c88f2fc8.json")
	if err != nil {
		fmt.Println("error opening file ", err)
		return
	}

	var events []HolodeckEvent

	position := 0
	for position < len(content) {
		_, _, newPosition := readTag(content, position)
		position = newPosition
		length, newPosition := readVarint(content, position)

		position = newPosition

		eventBytes := content[position : position+int(length)]
		position += int(length)

		events = append(events, parseEvent(eventBytes))
	}

	contiguousDuplicates(events)

	clusteredDupes(events)

	frequencyOfActionByKeyValuePair(events, "join")

	frequencyOfActionByKeyValuePair(events, "leave")

	peekUpdates(events)
}
