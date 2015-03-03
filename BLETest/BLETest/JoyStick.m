//
//  LeftJoyStick.m
//  BLETest
//
//  Created by Robert Mannuzza on 1/24/15.
//  Copyright (c) 2015 Robert Mannuzza. All rights reserved.
//

#import "JoyStick.h"
#import "ThrottleImages.h"

@implementation JoyStick


// Only override drawRect: if you perform custom drawing.
// An empty implementation adversely affects performance during animation.
- (void)drawRect:(CGRect)rect {
    // Drawing code
    [ThrottleImages drawJoyStickWithHeight:self.height windowHeight:self.frame.size.height];
}

- (void)drawRect:(CGRect)rect withHeight:(CGFloat)height {
    // Drawing code
    [ThrottleImages drawJoyStickWithHeight:height windowHeight:self.frame.size.height];
}

-(UInt8)getDirection {
    if (self.height > 0.5) {
        return 0x02;         //reverse
    } else if (self.height < 0.5) {
        return 0x01;         //forward
    }
    return 0x00;
}

-(UInt8)getSpeed {
    CGFloat temp = (self.height * 256);
    if ((temp > 0) && (temp < 31)){
        return 0xFE;                    //255
    }
    if ((temp > 32) && (temp < 63)){
        return 0xBF;                    //191
    }
    if ((temp > 64) && (temp < 95)){
        return 0x7F;                    //127
    }
    if ((temp > 96) && (temp < 127)){
        return 0x3F;                    //63
    }
    if (temp == 128){
        return 0x00;                    //00
    }
    if ((temp > 129) && (temp < 159)){
        return 0x3F;                    //63
    }
    if ((temp > 160) && (temp < 191)){
        return 0x7F;                    //127
    }
    if ((temp > 192) && (temp < 223)){
        return 0xBF;                    //191
    }
    if ((temp > 224) && (temp < 256)){
        return 0xFE;                    //255
    }
    return 0;
}

@end
