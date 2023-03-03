//
//  RBColor.cpp
//
//  Created by Roger Boesch on 30.07.18.
//  Copyright © 2018 Roger Boesch. All rights reserved.
//

#include "RBColor.hpp"
#include "RBBase.hpp"
#include "RBLogger.hpp"

#include <string>
#include <math.h>
#include <vector>

namespace rb {
    
    Color Color::Black =     Color("#000000");
    Color Color::White =     Color("#FFFFFF");
    Color Color::Gray =      Color("#8E8E93");
    Color Color::GrayLight = Color("#CCCCCC");

    Color Color::Red =       Color("#FF3824");
    Color Color::Green =     Color("#44DB5E");
    Color Color::Blue =      Color("#0076FF");
    Color Color::Cyan =      Color("#54C7fC");
    Color Color::Yellow =    Color("#FFCD00");
    Color Color::Orange =    Color("#FF9600");
    Color Color::Purple =    Color("#FF2851");

    // -----------------------------------------------------------------------------
    
    std::vector<Color> *colorPalette2 = new std::vector<Color>();
    std::string colorList2[] = {
        // Gray colors
        "#C0CBCB","#CAC6BF","#B4ACA1","#E5E4dE","#C5C1B7",
        
        // Red colors
        "#800000","#AA0000","#D40000","#FF0000","#FF2A2A",

        // Green colors
        "#075907","#097609","#70AF1A","#B9D40B","#E5EB0B",

        // Purple
        "#660067","#810080","#BE2AED","#D997FF","#F0BCFF",
        
        // Neon
        "#FF15AC","#FF6900","#FFFF01","#00FF01","#01E6f0"
    };

    std::vector<Color> *colorPalette1 = new std::vector<Color>();

    std::string colorList1[] = {
        "#EFE8D7", "#D7CEBD", "#BAB1A0", "#9D9382", "#827664", "#615645", "#39302C", "#000000",
        "#DD4F25", "#C1411F", "#AA3018", "#902012", "#6C160B", "#4A0D07", "#1D0302", "#100204",
        "#E7A145", "#CF8D36", "#B37628", "#956121", "#764A16", "#56310E", "#2D0502", "#000002",
        "#F3CB65", "#D4B34A", "#BD9A39", "#9E7E28", "#81621E", "#654613", "#332005", "#382006",
        "#FDF584", "#E9D66B", "#D0B951", "#B19C45", "#937926", "#6C5719", "#44320B", "#45340C",
        "#9DB94A", "#8DA33F", "#828E2E", "#6B7722", "#5B631D", "#4A4813", "#302B07", "#342D0A",
        "#597721", "#526A1C", "#4B5B17", "#435014", "#394213", "#2F330A", "#292305", "#272105",
        "#A1E3BA", "#8ECCA1", "#7DB086", "#6F9674", "#5E7A5B", "#4C5F46", "#3B422C", "#3D402B",
        
        "#98D5D7", "#81BEC1", "#76AAA5", "#6A908B", "#5E7774", "#4C6059", "#42473C", "#44463C",
        "#658FC2", "#6080B4", "#59769D", "#4F678B", "#495970", "#414A59", "#3F3E41", "#3F3D40",
        "#6C6BC0", "#6463B1", "#605D9F", "#595386", "#544B75", "#4F4759", "#493E43", "#4B3F45",
        "#7961C7", "#725AB1", "#68568F", "#655089", "#5E4A71", "#5A475E", "#544349", "#504147",
        "#8241C4", "#7E38AA", "#7735A1", "#6D338D", "#653371", "#5F3D67", "#5B4151", "#5C404F",
        "#CC60CC", "#B756BA", "#A84AA0", "#9C4893", "#874679", "#774867", "#6B4B55", "#6C4952",
        "#E69B9A", "#DC9390", "#C28681", "#B27D79", "#986D66", "#8F6863", "#795E51", "#765D50",
        "#F7D1BD", "#E5C0A8", "#D4B295", "#C19E80", "#AE9073", "#987B66", "#7B6A53", "#7E6D57",
        
        "#FBE8D4", "#EFD6C2", "#DCC4AB", "#C5B19A", "#B59E88", "#9E8B75", "#8A7764", "#8A7767",
        "#688FC7", "#81A0CD", "#98B0D6", "#ACC0DC", "#BECEE0", "#D6DFE6", "#EBEFEB", "#FDFCF0",
        "#F1DC78", "#DFC769", "#CDB454", "#B99C41", "#A98933", "#966E23", "#81591D", "#6A4013",
        "#86A89E", "#799286", "#737C72", "#71665A", "#73513D", "#773822", "#7D200F", "#8B1E12",
        "#C0B872", "#B6AC70", "#A19F61", "#959357", "#878957", "#767B4F", "#6B714D", "#5C6443",
        "#A4B6D0", "#ABB8B9", "#B2B7A0", "#B8B989", "#BEB978", "#C8BB64", "#D6C15D", "#DDC25A",
        "#926724", "#895E1E", "#7E591F", "#784F18", "#724916", "#684114", "#623E12", "#5B330E",
        "#B29234", "#B08D38", "#AD8B39", "#AC8A3A", "#A78738", "#A58642", "#A48543", "#A1824A",
        
        "#D1BA4E", "#D8C05F", "#DDCD6E", "#E6D586", "#F2E4AA", "#F6EDC1", "#FCF5D7", "#FEFBEF",
        "#B4441D", "#A75530", "#9C6B48", "#998666", "#979B7C", "#98B698", "#9ECDB2", "#B0EBCF",
        "#C56F28", "#B9773B", "#AA8758", "#A99367", "#9FA07D", "#9DAC9B", "#9DBAAF", "#A1CBC5",
        "#D59A3D", "#C39B46", "#BD9C57", "#B49F74", "#ACA184", "#A6A59A", "#9FAAAF", "#9CADC2",
        "#DFC85B", "#D6C05C", "#CDB668", "#C5AC75", "#B9A386", "#AE9D99", "#A397A9", "#9D90C8",
        "#F8F484", "#EAE074", "#DBCD70", "#D0BC78", "#C2A783", "#B3989B", "#AE86B0", "#A576C8",
        "#828282", "#717171", "#5F5F5F", "#4C4C4C", "#3D3D3B", "#272727", "#11120F", "#010101",
        "#FFFFFF", "#F0F0F0", "#DFDFDF", "#D3D3D3", "#C3C3C3", "#B3B3B3", "#A4A4A4", "#959595"
    };
    
    // -------------------------------------------------------------------------
    
    Color::Color(std::string hex) {
        *this = Color::hexToRGB(hex);
    }
    
    // -------------------------------------------------------------------------
    
    Color Color::alpha(float alpha) {
        return Color(r, g, b, alpha);
    }
    
    // -------------------------------------------------------------------------
    
    void Color::createPalette() {
        if (colorPalette1->size() == 0) {
            for (int i = 0; i < 256; i++) {
                std::string str = colorList1[i];
                Color color = Color(str);
                colorPalette1->push_back(color);
            }
            
            Logger::debug("Color palette 1 created with ", colorPalette1->size(), " colors");
        }
        
        if (colorPalette2->size() == 0) {
            for (int i = 0; i < 25; i++) {
                std::string str = colorList2[i];
                Color color = Color(str);
                colorPalette2->push_back(color);
            }
            
            Logger::debug("Color palette 2 created with ", colorPalette2->size(), " colors");
        }
    }
    
    // -------------------------------------------------------------------------
    
    Color Color::random() {
        createPalette();
        
        int rnd = randomMax((int)colorPalette2->size());
        Color color = colorPalette2->at(rnd);
        
        return color;
    }
    
    Color Color::randomRange1(int min, int max) {
        createPalette();
        
        if (min < 0 || min > (int)colorPalette1->size()-1) {
            return Color::Red;
        }
        
        if (max < 0 || max > (int)colorPalette1->size()-1) {
            return Color::Red;
        }
        
        int rnd = rb::randomRange(min, max);
        Color color = colorPalette1->at(rnd);
        
        return color;
    }
    
    Color Color::randomRange2(int min, int max) {
        createPalette();
        
        if (min < 0 || min > (int)colorPalette2->size()-1) {
            return Color::Red;
        }
        
        if (max < 0 || max > (int)colorPalette2->size()-1) {
            return Color::Red;
        }
        
        int rnd = rb::randomRange(min, max);
        Color color = colorPalette2->at(rnd);
        
        return color;
    }
    
    Color Color::randomNeon() {
        createPalette();
        int rnd = rb::randomRange(20, 24);
        Color color = colorPalette2->at(rnd);
        
        return color;
    }

    Color Color::randomGrayscale() {
        createPalette();
        int rnd = rb::randomRange(0, 4);
        Color color = colorPalette2->at(rnd);
        
        return color;
    }

    Color Color::randomGreen() {
        createPalette();
        int rnd = rb::randomRange(10, 14);
        Color color = colorPalette2->at(rnd);
        
        return color;
    }

    // -------------------------------------------------------------------------
    
    Color Color::palette1(int index) {
        createPalette();
        
        if (index < 0 || index > (int)colorPalette1->size()-1) {
            return Color::Red;
        }
        
        return colorPalette1->at(index);
    }

    // -------------------------------------------------------------------------
    
    Color Color::palette2(int index) {
        createPalette();
        
        if (index < 0 || index > (int)colorPalette2->size()-1) {
            return Color::Red;
        }
        
        return colorPalette2->at(index);
    }

    // -------------------------------------------------------------------------
    
    int Color::hexToDec(std::string hex) {
        int hexLength = (int)hex.length();
        double dec = 0;
        
        for (int i = 0; i < hexLength; ++i)
        {
            char b = hex[i];
            
            if (b >= 48 && b <= 57)
                b -= 48;
            else if (b >= 65 && b <= 70)
                b -= 55;
            
            dec += b * pow(16, ((hexLength - i) - 1));
        }
        
        return (int)dec;
    }
    
    // -------------------------------------------------------------------------
    
    Color Color::hexToRGB(std::string hex) {
        if (hex[0] == '#')
            hex = hex.erase(0, 1);
        
        int r = (unsigned char)hexToDec(hex.substr(0, 2));
        int g = (unsigned char)hexToDec(hex.substr(2, 2));
        int b = (unsigned char)hexToDec(hex.substr(4, 2));
        
        int a = 255;
        if (hex.length() > 6) {
            a = (unsigned char)hexToDec(hex.substr(6, 2));
        }
        return Color(r/255.0f, g/255.0f, b/255.0f, a/255.0f);
    }
    
}

