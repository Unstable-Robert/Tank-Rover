//
//  ThrottleImages.m
//  BluetoothTank
//
//  Created by Robert Mannuzza on 1/24/15.
//  Copyright (c) 2015 Me . All rights reserved.
//
//  Generated by PaintCode (www.paintcodeapp.com)
//

#import "ThrottleImages.h"


@implementation ThrottleImages

#pragma mark Cache

static UIColor* _pathWay = nil;
static UIColor* _insideHandle = nil;
static UIColor* _outSidePathWay = nil;
static UIColor* _outSideHandle = nil;

static NSShadow* _shadow = nil;

#pragma mark Initialization

+ (void)initialize
{
    // Colors Initialization
    _pathWay = [UIColor colorWithRed: 0.349 green: 0.349 blue: 0.349 alpha: 1];
    _insideHandle = [UIColor colorWithRed: 1 green: 1 blue: 1 alpha: 1];
    _outSidePathWay = [UIColor colorWithRed: 0.815 green: 0.815 blue: 0.815 alpha: 1];
    _outSideHandle = [UIColor colorWithRed: 0.5 green: 0.5 blue: 0.5 alpha: 1];

    // Shadows Initialization
    _shadow = [NSShadow shadowWithColor: [UIColor.blackColor colorWithAlphaComponent: 0.59] offset: CGSizeMake(3.1, 3.1) blurRadius: 13];

}

#pragma mark Colors

+ (UIColor*)pathWay { return _pathWay; }
+ (UIColor*)insideHandle { return _insideHandle; }
+ (UIColor*)outSidePathWay { return _outSidePathWay; }
+ (UIColor*)outSideHandle { return _outSideHandle; }

#pragma mark Shadows

+ (NSShadow*)shadow { return _shadow; }

#pragma mark Drawing Methods

+ (void)drawJoyStickWithHeight: (CGFloat)height windowHeight: (CGFloat)windowHeight
{
    //// General Declarations
    CGContextRef context = UIGraphicsGetCurrentContext();


    //// Shadow Declarations
    NSShadow* slotShadow = [NSShadow shadowWithColor: UIColor.blackColor offset: CGSizeMake(0.1, -0.1) blurRadius: 5];

    //// Variable Declarations
    CGFloat slotHeight = windowHeight - 40;
    CGFloat yValue = slotHeight * height;
    CGFloat middleBarX = windowHeight / 2.0;

    //// Group 2
    {
        //// Rectangle Drawing
        UIBezierPath* rectanglePath = [UIBezierPath bezierPathWithRoundedRect: CGRectMake(126, 0, 59, windowHeight) cornerRadius: 14];
        [ThrottleImages.outSidePathWay setFill];
        [rectanglePath fill];


        //// Bezier Drawing
        CGContextSaveGState(context);
        CGContextTranslateCTM(context, 125.87, middleBarX);

        UIBezierPath* bezierPath = UIBezierPath.bezierPath;
        [bezierPath moveToPoint: CGPointMake(0, 0)];
        [bezierPath addLineToPoint: CGPointMake(59.26, 0)];
        [UIColor.blackColor setStroke];
        bezierPath.lineWidth = 1;
        [bezierPath stroke];

        CGContextRestoreGState(context);


        //// Rectangle 2 Drawing
        UIBezierPath* rectangle2Path = [UIBezierPath bezierPathWithRect: CGRectMake(141, 20, 29, slotHeight)];
        CGContextSaveGState(context);
        CGContextSetShadowWithColor(context, slotShadow.shadowOffset, slotShadow.shadowBlurRadius, [slotShadow.shadowColor CGColor]);
        [ThrottleImages.pathWay setFill];
        [rectangle2Path fill];
        CGContextRestoreGState(context);

    }


    //// Group
    {
        CGContextSaveGState(context);
        CGContextTranslateCTM(context, 53, yValue);



        //// Oval Drawing
        UIBezierPath* ovalPath = [UIBezierPath bezierPathWithOvalInRect: CGRectMake(0, 0, 206, 40)];
        CGContextSaveGState(context);
        CGContextSetShadowWithColor(context, ThrottleImages.shadow.shadowOffset, ThrottleImages.shadow.shadowBlurRadius, [ThrottleImages.shadow.shadowColor CGColor]);
        [ThrottleImages.outSideHandle setFill];
        [ovalPath fill];
        CGContextRestoreGState(context);



        //// Oval 2 Drawing
        UIBezierPath* oval2Path = [UIBezierPath bezierPathWithOvalInRect: CGRectMake(11, 5, 184, 29)];
        [ThrottleImages.insideHandle setFill];
        [oval2Path fill];



        CGContextRestoreGState(context);
    }
}

@end



@implementation NSShadow (PaintCodeAdditions)

- (instancetype)initWithColor: (UIColor*)color offset: (CGSize)offset blurRadius: (CGFloat)blurRadius
{
    self = super.init;
    if (self)
    {
        self.shadowColor = color;
        self.shadowOffset = offset;
        self.shadowBlurRadius = blurRadius;
    }
    return self;
}

+ (instancetype)shadowWithColor: (UIColor*)color offset: (CGSize)offset blurRadius: (CGFloat)blurRadius
{
    return [self.alloc initWithColor: color offset: offset blurRadius: blurRadius];
}

- (void)set
{
    CGContextSetShadowWithColor(UIGraphicsGetCurrentContext(), self.shadowOffset, self.shadowBlurRadius, [self.shadowColor CGColor]);
}

@end
