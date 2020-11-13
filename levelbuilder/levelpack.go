package main

import (
	"fmt"
	"os"
)

func writeSpecInclude(specInc string, levels []string) error {
	output, err := os.OpenFile(specInc, os.O_CREATE|os.O_WRONLY|os.O_TRUNC, 0664)

	if err != nil {
		return err
	}

	defer output.Close()

	for _, level := range levels {
		output.WriteString(fmt.Sprintf("\tinclude \"%s\"\n", level))
	}

	return nil
}

func writeSpecSeg(specSeg string, levels []string) error {
	output, err := os.OpenFile(specSeg, os.O_CREATE|os.O_WRONLY|os.O_TRUNC, 0664)

	if err != nil {
		return err
	}

	defer output.Close()

	for _, level := range levels {
		output.WriteString(fmt.Sprintf(`
beginseg
	name "%s"
	flags OBJECT
	number LEVEL_SEGMENT
	include "src/levels/%s/geo.o"
endseg
`, level, level))
	}

	return nil
}

func writeLevelPack(cName string, themeIndex string, geoOutput string, levels []string) error {
	output, err := os.OpenFile(geoOutput, os.O_CREATE|os.O_WRONLY|os.O_TRUNC, 0664)

	if err != nil {
		return err
	}

	defer output.Close()

	output.WriteString(`
#include "levels.h"
#include "src/levelthemes/allthemes.h"
`)

	for _, level := range levels {
		output.WriteString(fmt.Sprintf("#include \"src/levels/%s/header.h\"\n", level))
	}

	output.WriteString("\n")

	for _, level := range levels {
		output.WriteString(fmt.Sprintf("extern char _%sSegmentRomStart[], _%sSegmentRomEnd[];\n", level, level))
	}

	output.WriteString(fmt.Sprintf(`
struct LevelDefinition _level_group_%s[] = {
`, cName))

	for _, level := range levels {
		output.WriteString(fmt.Sprintf("    {_%sSegmentRomStart, _%sSegmentRomEnd, &_level_%s_levelData, &gAllThemes[%s]},\n", level, level, level, themeIndex))
	}

	output.WriteString(fmt.Sprintf("};\n\nint _level_group_%s_count = sizeof(_level_group_%s) / sizeof(*_level_group_%s);\n", cName, cName, cName))

	return nil
}

func writeLevelPackHeader(cName string, geoOutput string, levels []string) error {
	var headerFilename = geoOutput[0:len(geoOutput)-2] + ".h"

	output, err := os.OpenFile(headerFilename, os.O_CREATE|os.O_WRONLY|os.O_TRUNC, 0664)

	if err != nil {
		return err
	}

	defer output.Close()

	output.WriteString(fmt.Sprintf(`
#ifndef _LEVELS_%s_H
#define _LEVELS_%s_H

#include "src/level/level.h"

extern struct LevelDefinition _level_group_%s[];
extern int _level_group_%s_count; 

#endif
`, cName, cName, cName, cName))

	return nil
}

func ProcessLevelPack(cName string, themeIndex string, geoOutput string, specSeg string, specInc string, levels []string) error {
	err := writeLevelPack(cName, themeIndex, geoOutput, levels)
	if err != nil {
		return err
	}
	err = writeLevelPackHeader(cName, geoOutput, levels)
	if err != nil {
		return err
	}
	err = writeSpecInclude(specInc, levels)
	if err != nil {
		return err
	}
	err = writeSpecSeg(specSeg, levels)
	if err != nil {
		return err
	}
	return nil
}
