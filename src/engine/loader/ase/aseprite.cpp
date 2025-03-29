#include "aseprite.hpp"

#include "reader.hpp"

namespace loader::ase {
void aseprite(const std::string filepath, Asefile &f) {
    Reader r{filepath};

    dword filesize = r.rdword();
    word magicNumber = r.rword();
    word frames = r.rword();
    word width = r.rword();
    word height = r.rword();
    word imageType = r.rword();
    dword flags = r.rdword();
    word speed_d = r.rword();
    r.rpadding(2 * sizeof(dword));
    byte transIndex = r.rbyte();
    r.rpadding(3);
    word colorCount = r.rword();
    colorCount = colorCount == 0 ? 255 : colorCount;
    byte pixelWidth = r.rbyte();
    byte pixelHeight = r.rbyte();
    int pixelRatio = (pixelWidth == 0 || pixelHeight == 0) ? 1 : pixelWidth / pixelHeight;
    int xPos = r.rshort();
    int yPos = r.rshort();
    word gridWidth = r.rword();
    gridWidth = gridWidth == 0 ? 16 : gridWidth;
    word gridHeight = r.rword();
    gridHeight = gridHeight == 0 ? 16 : gridHeight;
    r.rpadding(84);

    std::vector<std::vector<std::vector<uint32_t>>> images(frames);
    std::vector<SDL_Color> colorPalette;

    f.durations.resize(frames);

    for (int i = 0; i < frames; i++) {
        dword bytes = r.rdword();
        word magicNumber = r.rword();
        word oldChunkCount = r.rword();
        word duration = r.rword();
        f.durations[i] = duration;
        r.rpadding(2);
        dword chunkCount = r.rdword();
        if (chunkCount > 10000) {
            break;
        }

        for (int j = 0; j < chunkCount; j++) {
            size_t chunkPos = r.tell();
            dword chunkSize = r.rdword();
            word chunkType = r.rword();
            switch (chunkType) {
                case 0x0004: {
                    word packetCount = r.rword();
                    for (int k = 0; k < packetCount; k++) {
                        byte paletteEntries = r.rbyte();
                        byte colorCount = r.rbyte();
                        for (int l = 0; l < colorCount; l++) {
                            byte red = r.rbyte();
                            byte g = r.rbyte();
                            byte b = r.rbyte();
                            colorPalette.push_back({red, g, b, 255});
                        }
                    }
                } break;
                case 0x0011: {
                    word packetCount = r.rword();
                    for (int k = 0; k < packetCount; k++) {
                        byte paletteEntries = r.rbyte();
                        byte colorCount = r.rbyte();
                        for (int l = 0; l < colorCount; l++) {
                            byte red = r.rbyte();
                            byte g = r.rbyte();
                            byte b = r.rbyte();
                        }
                    }
                } break;
                case 0x2004: {
                    word flags = r.rword();
                    word layerType = r.rword();
                    word childLevel = r.rword();
                    word defaultLayerWidth = r.rword();
                    word defaultLayerHeight = r.rword();
                    word blendMode = r.rword();
                    byte opacity = r.rbyte();
                    r.rpadding(3);
                    std::string layerName = r.rstring();
                } break;
                case 0x2005: {
                    word layerIndex = r.rword();
                    short xPos = r.rshort();
                    short yPos = r.rshort();
                    byte opacityLevel = r.rbyte();
                    word celType = r.rword();
                    short zIndex = r.rshort();
                    r.rpadding(5);
                    switch (celType) {
                        case 0: {
                            word width = r.rword();
                            word height = r.rword();
                        } break;
                        case 1: {
                            word framePos = r.rword();
                        } break;
                        case 2: {
                            word width = r.rword();
                            word height = r.rword();
							std::vector<std::vector<uint32_t>> img;
                            r.rcompressed_image_rgba(width, height, chunkSize + chunkPos, img);
							images[i].resize(yPos + height);
							for (int y = 0; y < height; y++) {
								images[i][y + yPos].resize(xPos + width);
								for (int x = 0; x < width; x++) {
									images[i][yPos + y][xPos + x] = img[y][x];
								}
							}
                        } break;
                        case 3: {
                        } break;
                    }
                } break;
                case 0x2006: {
                    dword flags = r.rdword();
                    fixed xPos = r.rfixed();
                    fixed yPos = r.rfixed();
                    fixed w = r.rfixed();
                    fixed h = r.rfixed();
                    r.rpadding(16);
                } break;
                case 0x2007: {
                    word type = r.rword();
                    word flags = r.rword();
                    fixed gamma = r.rfixed();
                    r.rpadding(8);
                    switch (type) {
                        case 0: {
                        };
                        case 1: {
                        } break;
                        case 2: {
                            dword dataLength = r.rdword();
                            for (int k = 0; k < dataLength; k++) {
                                byte data = r.rbyte();
                            }
                        } break;
                    }
                } break;
                case 0x2008: {
                    dword entryCount = r.rdword();
                    r.rpadding(8);
                    for (int k = 0; k < entryCount; k++) {
                        dword id = r.rdword();
                        byte type = r.rbyte();
                        switch (type) {
                            case 0: {
                            } break;
                            case 1: {
                            } break;
                            case 2: {
                            } break;
                            case 3: {
                            } break;
                        }
                    }
                    r.rpadding(7);
                    std::string filename = r.rstring();
                } break;
                case 0x2016: { // deprecated
                    short xPos = r.rshort();
                    short yPos = r.rshort();
                    word w = r.rword();
                    word h = r.rword();
                    r.rpadding(8);
                    std::string maskName = r.rstring();
                    for (int k = 0; k < h * ((w + 7) / 8); k++) {
                        byte bitmapData = r.rbyte();
                    }
                } break;
                case 0x2017: {
                    // never used
                } break;
                case 0x2018: {
                    word tagCount = r.rword();
                    f.tags.resize(tagCount);
                    r.rpadding(8);
                    for (int k = 0; k < tagCount; k++) {
                        word fromFrame = r.rword();
                        word toFrame = r.rword();
                        byte loopAnimationDirection = r.rbyte();
                        word repeatCount = r.rword();
                        r.rpadding(6);
                        byte tagR = r.rbyte();
                        byte tagG = r.rbyte();
                        byte tagB = r.rbyte();
                        byte extraByte = r.rbyte();
                        std::string tagName = r.rstring();

                        f.tags[k] = {fromFrame, toFrame};
                    }
                } break;
                case 0x2019: {
                    dword paletteSize = r.rdword();
                    dword firstIndex = r.rdword();
                    dword lastIndex = r.rdword();
                    r.rpadding(8);
                    for (int k = firstIndex; k < lastIndex + 1; k++) {
                        word flags = r.rword();
                        byte red = r.rbyte();
                        byte g = r.rbyte();
                        byte b = r.rbyte();
                        byte a = r.rbyte();
                        colorPalette.push_back({red, g, b, a});
                        if (flags == 1) {
                            std::string colorName = r.rstring();
                        }
                    }
                } break;
                case 0x2020: {
                    dword flags = r.rdword();
                    if (flags == 1) {
                        std::string text = r.rstring();
                    } else if (flags == 2) {
                        byte red = r.rbyte();
                        byte g = r.rbyte();
                        byte b = r.rbyte();
                        byte a = r.rbyte();
                    } else if (flags == 4) {
                        dword propSize = r.rdword();
                        dword propMapCount = r.rdword();
                        for (int k = 0; k < propMapCount; k++) {
                            dword key = r.rdword();
                            dword propCount = r.rdword();
                            for (int l = 0; l < propCount; l++) {
                                std::string name = r.rstring();
                                word type = r.rword();
                                switch (type) {
                                    case 0x0001: {
                                        byte tbool = r.rbyte();
                                    } break;
                                    case 0x0002: {
                                        int8_t t = r.rbyte();
                                    } break;
                                    case 0x0003: {
                                        uint8_t t = r.rbyte();
                                    } break;
                                    case 0x0004: {
                                        int16_t t = r.rshort();
                                    } break;
                                    case 0x0005: {
                                        uint16_t t = r.rword();
                                    } break;
                                    case 0x0006: {
                                        int32_t t = r.rlong();
                                    } break;
                                    case 0x0007: {
                                        uint32_t t = r.rdword();
                                    } break;
                                    case 0x0008: {
                                        int64_t t = r.rlong64();
                                    } break;
                                    case 0x0009: {
                                        uint64_t t = r.rqword();
                                    } break;
                                    case 0x000A: {
                                        fixed t = r.rfixed();
                                    } break;
                                    case 0x000B: {
                                        float t = r.rfloat();
                                    } break;
                                    case 0x000C: {
                                        double t = r.rdouble();
                                    } break;
                                    case 0x000D: {
                                        std::string t = r.rstring();
                                    } break;
                                    case 0x000E: {
                                        Point t = r.rpoint();
                                    } break;
                                    case 0x000F: {
                                        Size t = r.rsize();
                                    } break;
                                    case 0x0010: {
                                        Rect t = r.rrect();
                                    } break;
                                    case 0x0011: {
                                        dword count = r.rdword();
                                        word type = r.rword();
                                        if (type == 0) {
                                            for (int m = 0; m < count; m++) {
                                                // IDK WHAT TO DO HERE
                                                word t = r.rword();
                                            }
                                        } else {
                                            for (int m = 0; m < count; m++) {
                                                // AGAIN IDK WHAT TO DO IM SCARED
                                            }
                                        }
                                    } break;
                                    case 0x0012: {
                                    } break;
                                    case 0x0013: {
                                        UUID u = r.ruuid();
                                    } break;
                                }
                            }
                        }
                    }
                } break;
                case 0x2022: {
                    dword keyCount = r.rdword();
                    dword flags = r.rdword();
                    dword reserved = r.rdword();
                    std::string name = r.rstring();
                    for (int k = 0; k < keyCount; k++) {
                        dword frameNumber = r.rdword();
                        long x = r.rlong();
                        long y = r.rlong();
                        dword w = r.rdword();
                        dword h = r.rdword();
                        if (flags == 1) {
                            long cX = r.rlong();
                            long cY = r.rlong();
                            dword cW = r.rdword();
                            dword cH = r.rdword();
                        } else if (flags == 2) {
                            long pivX = r.rlong();
                            long pivY = r.rlong();
                        }
                    }
                } break;
                case 0x2023: {
                    dword tilesetID = r.rdword();
                    dword flags = r.rdword();
                    dword tileCount = r.rdword();
                    word w = r.rword();
                    word h = r.rword();
                    short baseIndex = r.rshort();
                    r.rpadding(14);
                    std::string tilesetName = r.rstring();
                    if (flags == 1) {
                        dword externFileId = r.rword();
                        dword externFileTilesetId = r.rdword();
                    } else if (flags == 2) {
                        dword length = r.rdword();
                    }
                } break;
            }
        }
    }

    f.frames.resize(frames);
    for (int i = 0; i < frames; i++) {
        uint32_t *pixels = (uint32_t *)malloc(width * height * sizeof(uint32_t));
		int pos = 0;
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
				if (y >= images[i].size() || x >= images[i][y].size()) {
					pixels[y * width + x] = 0;
				} else {
					pixels[y * width + x] = images[i][y][x];
				}
            }
        }
        f.frames[i] = SDL_CreateSurfaceFrom(width, height, SDL_PIXELFORMAT_RGBA32, pixels, width * 4);
    }
}
} // namespace loader::ase
