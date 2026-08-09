package main

import (
	"encoding/json"
	"fmt"
)

func firstAvTalking(events []HolodeckEvent) (uint64, bool) {
	for _, ev := range events {
		if ev.Event == "" {
			continue
		}
		var array []interface{}
		err := json.Unmarshal([]byte(ev.Event), &array)
		if err != nil {
			continue
		}

		if len(array) < 2 || array[0] != "signalling" {
			continue
		}

		payload, ok := array[1].(map[string]interface{})
		if !ok {
			continue
		}

		if payload["t"] == "av-talking" {
			return ev.Timestamp, true
		}
	}

	return 0, false
}

type audioChunks struct {
	start uint64
	end   uint64
}

func printBlocks(blocks []audioChunks) {
	for _, block := range blocks {
		fmt.Println("start : ", block.start, " end: ", block.end)
	}
}

func unpackUpdate(events []HolodeckEvent) {
	for _, ev := range events {
		if len(ev.Update) == 0 {
			continue
		}
		fmt.Println(">> ", string(ev.Update))
		var array []interface{}
		err := json.Unmarshal([]byte(ev.Update), &array)
		if err != nil {
			continue
		}
		fmt.Println("update is ", array[0])
	}
}

func allAvTalkingBlocks(events []HolodeckEvent) []audioChunks {
	var blocks []audioChunks
	var start uint64

	for _, ev := range events {
		if ev.Event == "" {
			continue
		}

		payload := extractEventType(ev, "signalling")
		if payload == nil {
			continue
		}

		if payload["t"] == "av-talking" {
			if start == 0 {
				start = ev.Timestamp
			}
		} else if start != 0 {
			block := audioChunks{
				start: start,
				end:   ev.Timestamp,
			}
			start = 0
			blocks = append(blocks, block)
		}
	}
	printBlocks(blocks)
	//	fmt.Println("block count ", len(blocks))
	return blocks
}
