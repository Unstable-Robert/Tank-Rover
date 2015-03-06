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
    CGFloat temp = (self.height * 100) + 100;
    if ((temp >= 190) && (temp <= 200)){
        return 0xC8;                    //200
    }
    if ((temp >= 100) && (temp <= 110)){
        return 0x64;                    //100
    }
    return temp;
}

@end
