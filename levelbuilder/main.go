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
levelbuilder levelpack cName themeIndex outputDir/levels.c spec_level_segments spec_level_includes inputFile.level levelName0 levelName1 levelName2
`)
}

func main() {
	if len(os.Args) < 2 {
		printUsage()
	} else if os.Args[1] == "level" {
		if len(os.Args) == 5 {
			processLevel(os.Args[2], os.Args[3], os.Args[4], 4)
		} else {
			printUsage()
		}
	} else if os.Args[1] == "collision" {
		if len(os.Args) == 5 {
			ConvertCollisionMesh(os.Args[2], os.Args[3], os.Args[4])
		} else {
			printUsage()
		}
	} else if os.Args[1] == "levelpack" {
		if len(os.Args) >= 7 {
			err := ProcessLevelPack(os.Args[2], os.Args[3], os.Args[4], os.Args[5], os.Args[6], os.Args[7:len(os.Args)])

			if err != nil {
				log.Fatal(err)
			}

			log.Printf("Write level dev %s", os.Args[3])
		} else {
			printUsage()
		}
	} else {
		printUsage()
	}
}
