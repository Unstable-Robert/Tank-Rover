//
//  CompassView.m
//  BLETest
//
//  Created by Robert Mannuzza on 7/13/15.
//  Copyright © 2015 Robert Mannuzza. All rights reserved.
//

#import "CompassView.h"
#import "StyleKit.h"

@implementation CompassView
- (void)drawRect:(CGRect)rect {
    // Drawing code
    [StyleKit drawCompassWithAngle:self.angle];
}

- (void)drawRect:(CGRect)rect withAngle:(CGFloat)angle {
    // Drawing code
    [StyleKit drawCompassWithAngle:angle];
}
/*
// Only override drawRect: if you perform custom drawing.
// An empty implementation adversely affects performance during animation.
- (void)drawRect:(CGRect)rect {
    // Drawing code
}
*/

@end
