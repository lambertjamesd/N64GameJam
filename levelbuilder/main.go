package main

import (
	"log"
	"os"
)

func printUsage() {
	log.Print(`
Usage:
levelbuilder level cName inputFile.level outputDir/geo.c
levelbuilder collision cName inputFile.ply outputDir/geo.inc.c
`)
}

func main() {
	if len(os.Args) < 2 {
		printUsage()
	} else if os.Args[1] == "level" {
		if len(os.Args) == 5 {
			processLevel(os.Args[2], os.Args[3], os.Args[4], 8)
		} else {
			printUsage()
		}
	} else if os.Args[1] == "collision" {
		if len(os.Args) == 5 {
			ConvertCollisionMesh(os.Args[2], os.Args[3], os.Args[4])
		} else {
			printUsage()
		}
	} else {
		printUsage()
	}
}
