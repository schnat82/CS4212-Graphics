/*
 *  handleGraphicsArgs.cpp
 *
 *  Created by Pete Willemsen on 10/6/09.
 *  Copyright 2009 Department of Computer Science, University of Minnesota-Duluth. All rights reserved.
 *
 * This file is part of libSIVELab (libsivelab).
 *
 * libsivelab is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libsivelab is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with libsivelab.  If not, see <http://www.gnu.org/licenses/>.
 */


#include "handleGraphicsArgs.h"

#include <cstdlib>
#include <iostream>

using namespace sivelab;

GraphicsArgs::GraphicsArgs()
    : verbose(false),
      windowWidth(default_WindowSize),
      windowHeight(default_WindowSize),
      width(100),
      height(100),
      aspectRatio(1.0f),
      useShadow(true),
      bgColor{0.0f, 0.0f, 0.0f},
      useDepthOfField(false),
      depthOfFieldDistance(0.0f),
      numCpus(1),
      rpp(1),
      recursionDepth(4),
      splitMethod("objectMedian"),
      withPreview(false),
      withIntersectTest(false),
      withGridDim(false),
      gridDimension(0)
{
    reg("help",
        "help/usage information",
        ArgumentParsing::NONE, '?');

    reg("verbose",
        "turn on verbose output",
        ArgumentParsing::NONE, 'v');

    reg("inputfile",
        "input file name to use",
        ArgumentParsing::STRING, 'i');

    reg("outputfile",
        "output PNG file name",
        ArgumentParsing::STRING, 'o');

    reg("numcpus",
        "number of CPU threads",
        ArgumentParsing::INT, 'n');

    reg("width",
        "width of output image (default 100)",
        ArgumentParsing::INT, 'w');

    reg("height",
        "height of output image (default 100)",
        ArgumentParsing::INT, 'h');

    reg("aspect",
        "aspect ratio of image (width/height)",
        ArgumentParsing::FLOAT, 'a');

    reg("depth",
        "depth of field focus distance",
        ArgumentParsing::FLOAT, 'd');

    reg("rpp",
        "total rays per pixel (default 1)",
        ArgumentParsing::INT, 'r');

    reg("recursionDepth",
        "maximum recursion depth (default 4)",
        ArgumentParsing::INT, 'k');

    reg("split",
        "BVH split method (default objectMedian)",
        ArgumentParsing::STRING, 's');

    reg("winwidth",
        "preview window width",
        ArgumentParsing::INT, 'x');

    reg("winheight",
        "preview window height",
        ArgumentParsing::INT, 'y');

    reg("with-preview",
        "enable OpenGL preview",
        ArgumentParsing::NONE, 'p');

    reg("with-grid-dim",
        "thread work grid dimension",
        ArgumentParsing::INT, 'g');

    reg("with-intersect-test",
        "intersection visualization",
        ArgumentParsing::NONE, 't');
}

void GraphicsArgs::process(int argc, char* argv[])
{
    processCommandLineArgs(argc, argv);

    if (isSet("help")) {
        printUsage();
        std::exit(EXIT_SUCCESS);
    }

    verbose = isSet("verbose");

    isSet("width", width);
    isSet("height", height);

    isSet("winwidth", windowWidth);
    isSet("winheight", windowHeight);

    // Calculate aspect ratio from the image dimensions.
    if (height > 0) {
        aspectRatio =
            static_cast<float>(width) / height;
    }

    // Allow an explicit aspect ratio override.
    isSet("aspect", aspectRatio);

    useDepthOfField =
        isSet("depth", depthOfFieldDistance);

    isSet("numcpus", numCpus);
    isSet("rpp", rpp);
    isSet("recursionDepth", recursionDepth);

    isSet("split", splitMethod);

    isSet("inputfile", inputFileName);
    isSet("outputfile", outputFileName);

    // These options are OFF unless explicitly provided.
    withPreview = isSet("with-preview");

    withGridDim =
        isSet("with-grid-dim", gridDimension);

    withIntersectTest =
        isSet("with-intersect-test");

    if (verbose) {
        std::cout
            << "Image dimensions: "
            << width << " x " << height << '\n'
            << "Aspect ratio: " << aspectRatio << '\n'
            << "Rays per pixel: " << rpp << '\n'
            << "Recursion depth: " << recursionDepth << '\n'
            << "CPU threads: " << numCpus << '\n'
            << "Output file: " << outputFileName << '\n'
            << "OpenGL preview: "
            << (withPreview ? "ON" : "OFF") << '\n';
    }
}
