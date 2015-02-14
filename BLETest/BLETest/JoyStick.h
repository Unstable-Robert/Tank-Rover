//
//  LeftJoyStick.h
//  BLETest
//
//  Created by Robert Mannuzza on 1/24/15.
//  Copyright (c) 2015 Robert Mannuzza. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface JoyStick : UIView
- (void)drawRect:(CGRect)rect withHeight:(CGFloat)height;

@property (nonatomic) CGFloat height;

-(UInt8)getDirection;
-(UInt8)getSpeed;

@end
