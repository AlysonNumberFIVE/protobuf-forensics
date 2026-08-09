package main

import (
	"encoding/json"
	"fmt"
	"sort"
)

// payloadKey isolates a holodeck even by whether its an event or an update.
func payloadKey(event HolodeckEvent) string {
	if event.Event != "" {
		return "event:" + event.Event
	}

	return "update:" + string(event.Update)
}

// contiguousDuplicates scans all HolodeckEvent objects and attempts to find  as many
// contiguous duplicate event types next to each other.
func contiguousDuplicates(events []HolodeckEvent) {
	total := 0
	byVerb := map[string]int{}

	for i := 1; i < len(events); i++ {
		if payloadKey(events[i]) == payloadKey(events[i-1]) {
			total++
			byVerb[eventVerb(events[i])]++
		}
	}

	fmt.Println("consecutive duplicate payloads:", total)
	for verb, n := range byVerb {
		fmt.Printf("  %-12s %d\n", verb, n)
	}
}

// clusteredDupes gathers and groups events that are duplicates of each other
// that are also right next to one another in clustered groups.
func clusteredDupes(events []HolodeckEvent) {
	// Gather every timestamp each payload appears at.
	times := map[string][]uint64{}
	for _, ev := range events {
		k := payloadKey(ev)
		times[k] = append(times[k], ev.Timestamp)
	}

	var findings []finding
	for k, ts := range times {
		if len(ts) < 2 {
			continue // appears once, nothing to say
		}
		sort.Slice(ts, func(a, b int) bool { return ts[a] < ts[b] })
		findings = append(findings, finding{k, len(ts), ts[len(ts)-1] - ts[0]})
	}

	// Sort by count, most-repeated first.
	sort.Slice(findings, func(a, b int) bool {
		return findings[a].count > findings[b].count
	})

	fmt.Println("\nrepeated payloads (count / span / density):")
	for _, f := range findings[:min(15, len(findings))] {
		perSec := "-"
		if f.span > 0 {
			perSec = fmt.Sprintf("%.1f/s", float64(f.count)/(float64(f.span)/1000))
		}
		fmt.Printf("  x%-4d span=%-7dms %-8s %.100s\n", f.count, f.span, perSec, f.key)
	}
}

// extractEventType returns the structure of a specific event type.
func extractEventType(event HolodeckEvent, eventType string) map[string]interface{} {
	var array []interface{}

	err := json.Unmarshal([]byte(event.Event), &array)
	if err != nil {
		return nil
	}

	if len(array) < 2 || array[0] != eventType {
		return nil
	}

	payload, ok := array[1].(map[string]interface{})
	if !ok {
		return nil
	}

	return payload
}

func frequencyOfActionByKeyValuePair(events []HolodeckEvent, action string) map[string]int64 {
	joinFrequency := map[string]int64{}

	for _, ev := range events {
		if ev.Event == "" {
			continue
		}
		payload := extractEventType(ev, action)
		if payload == nil {
			continue // not a join, or failed to extract
		}

		id, ok := payload["id"].(float64)
		if !ok {
			continue
		}

		joinFrequency[fmt.Sprintf("%.0f", id)]++
	}

	fmt.Println("frequency")
	for id, frequency := range joinFrequency {
		fmt.Printf("type: %s, id: %s, frequency: %d", action, id, frequency)
	}
	fmt.Println()
	return joinFrequency
}

func min(a, b int) int {
	if a < b {
		return a
	}
	return b
}
