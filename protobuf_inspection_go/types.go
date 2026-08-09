package main

type HolodeckEvent struct {
	Timestamp uint64
	Update    []byte
	Event     string
}

type finding struct {
	key   string
	count int
	span  uint64 // ms between first and last occurrence
}
