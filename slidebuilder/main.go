package main

import (
	"fmt"
	"log"
	"os"
)

func writeHeader(output string, slides []string) {
	outFile, err := os.OpenFile(output, os.O_TRUNC|os.O_CREATE|os.O_WRONLY, 0664)

	if err != nil {
		log.Fatal(err)
	}

	defer outFile.Close()

	outFile.WriteString(`#ifndef _SLIDES_HEADER_H
#define _SLIDES_HEADER_H

extern char* gAllSlideLocations[];
extern char* gAllSlideEndLocations[];
extern int gAllSlideCount;

`)

	for _, slide := range slides {
		outFile.WriteString(fmt.Sprintf("extern unsigned short %s_0_0;\n", slide))
	}

	outFile.WriteString("\n#endif\n")
}

func writeData(output string, headerPath string, slides []string) {
	outFile, err := os.OpenFile(output, os.O_TRUNC|os.O_CREATE|os.O_WRONLY, 0664)

	if err != nil {
		log.Fatal(err)
	}

	defer outFile.Close()

	outFile.WriteString(fmt.Sprintf(`
#include "%s"

`, headerPath))

	for _, slide := range slides {
		outFile.WriteString(fmt.Sprintf("extern char _%s_slideSegmentRomStart[];\nextern char _%s_slideSegmentRomEnd[];\n", slide, slide))
	}

	outFile.WriteString(`
char* gAllSlideLocations[] = {
`)

	for _, slide := range slides {
		outFile.WriteString(fmt.Sprintf("    _%s_slideSegmentRomStart,\n", slide))
	}

	outFile.WriteString(`};
	
char* gAllSlideEndLocations[] = {
`)

	for _, slide := range slides {
		outFile.WriteString(fmt.Sprintf("    _%s_slideSegmentRomEnd,\n", slide))
	}

	outFile.WriteString(fmt.Sprintf(`};

int gAllSlideCount = %d;	
`, len(slides)))
}

func writeSpecSegs(output string, slides []string) {
	outFile, err := os.OpenFile(output, os.O_TRUNC|os.O_CREATE|os.O_WRONLY, 0664)

	if err != nil {
		log.Fatal(err)
	}

	defer outFile.Close()

	for _, slide := range slides {
		outFile.WriteString(fmt.Sprintf(`
beginseg
	name "%s_slide"
	flags RAW
	include "imageslides/%s.jpeg"
endseg		
`, slide, slide))
	}
}

func writeSpecInclude(output string, slides []string) {
	outFile, err := os.OpenFile(output, os.O_TRUNC|os.O_CREATE|os.O_WRONLY, 0664)

	if err != nil {
		log.Fatal(err)
	}

	defer outFile.Close()

	for _, slide := range slides {
		outFile.WriteString(fmt.Sprintf("    include \"%s_slide\"\n", slide))
	}
}

// usage slidebuilder slides.h slide_segs slide_include slide_name0 slide_name1 slide_name2 ...
func main() {
	var slidesHeader = os.Args[1]
	var slideData = os.Args[2]
	var slideSegs = os.Args[3]
	var slideInclude = os.Args[4]
	var slideNames = os.Args[5:len(os.Args)]

	writeHeader(slidesHeader, slideNames)
	writeSpecSegs(slideSegs, slideNames)
	writeSpecInclude(slideInclude, slideNames)
	writeData(slideData, slidesHeader, slideNames)
}
