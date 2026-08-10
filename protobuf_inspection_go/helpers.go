package main

import (
	"encoding/json"
	"fmt"
)

func readVarint(buffer []byte, position int) (uint64, int) {
	var (
		result uint64
		shift  uint
	)

	for {
		b := buffer[position]
		position++

		result |= uint64(b&0x7f) << shift
		if b&0x80 == 0 {
			break
		}

		shift += 7
	}

	return result, position
}

func readTag(buffer []byte, position int) (fieldNumber int, wireType int, newPosition int) {
	tag, position := readVarint(buffer, position)
	fieldNumber = int(tag >> 3)
	wireType = int(tag & 0x07)
	return fieldNumber, wireType, position
}

func parseEvent(buffer []byte) HolodeckEvent {
	var event HolodeckEvent
	position := 0

	for position < len(buffer) {
		fieldNumber, wireType, newPosition := readTag(buffer, position)
		position = newPosition

		switch fieldNumber {
		case 1:
			if wireType != 0 {
				fmt.Printf("WIRE MISMATCH: field 1 (timestamp) expected varint, got wt=%d\n", wireType)
			}
			value, newPosition := readVarint(buffer, position)
			event.Timestamp = value
			position = newPosition
		case 2:
			if wireType != 2 {
				fmt.Printf("WIRE MISMATCH: field 2 (update) expected len-delim, got wt=%d\n", wireType)
			}
			length, newPosition := readVarint(buffer, position)
			position = newPosition
			event.Update = buffer[position : position+int(length)]
			position += int(length)
		case 3:
			if wireType != 2 {
				fmt.Printf("WIRE MISMATCH: field 3 (event) expected len-delim, got wt=%d\n", wireType)
			}
			length, newPosition := readVarint(buffer, position)
			position = newPosition
			event.Event = string(buffer[position : position+int(length)])
			position += int(length)
		default:
			fmt.Printf("UNKNOWN FIELD %d (wt=%d) — not in .proto\n", fieldNumber, wireType)
		}

	}

	return event
}

func eventVerb(event HolodeckEvent) string {
	if event.Event == "" {
		return ""
	}

	var array []interface{}
	err := json.Unmarshal([]byte(event.Event), &array)
	if err != nil {
		return "trash data"
	}

	if len(array) == 0 {
		return "none"
	}

	// every event seemingly has a "join" or "leave"
	verb, ok := array[0].(string)
	if !ok {
		return "non-string verb"
	}

	return verb
}

func peekUpdates(events []HolodeckEvent) {
	for _, ev := range events {
		if len(ev.Update) == 0 {
			continue
		}

		n := 20
		if len(ev.Update) < n {
			n = len(ev.Update)
		}

		// hex of the first n bytes
		fmt.Printf("len=%-4d  ", len(ev.Update))
		for i := 0; i < n; i++ {
			fmt.Printf("%02x ", ev.Update[i])
		}

		fmt.Print(" |")
		for i := 0; i < n; i++ {
			b := ev.Update[i]
			if b >= 0x20 && b < 0x7f {
				fmt.Printf("%c", b)
			} else {
				fmt.Print(".")
			}
		}
		fmt.Println("|")
	}
}
