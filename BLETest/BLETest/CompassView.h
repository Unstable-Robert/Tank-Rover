//
//  CompassView.h
//  BLETest
//
//  Created by Robert Mannuzza on 7/13/15.
//  Copyright © 2015 Robert Mannuzza. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface CompassView : UIView
@property (nonatomic) CGFloat angle;
- (void)drawRect:(CGRect)rect withAngle:(CGFloat)angle;
@end
