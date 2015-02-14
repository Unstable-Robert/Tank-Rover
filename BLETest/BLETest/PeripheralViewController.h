//
//  PeripheralViewController.h
//  BLETest
//
//  Created by Robert Mannuzza on 1/23/15.
//  Copyright (c) 2015 Robert Mannuzza. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "BLEAdapter.h"
#import "JoyStick.h"

@interface PeripheralViewController : UIViewController <UIGestureRecognizerDelegate>

- (void)setData:(BLEAdapter *)adapter selectedPeripheral:(CBPeripheral *)peripheral;

@property (strong, nonatomic) BLEAdapter *bleAdapter;
@property (strong, nonatomic) CBPeripheral *toConnect;
@property (strong, nonatomic) IBOutlet JoyStick *leftJoyStick;
@property (strong, nonatomic) IBOutlet JoyStick *rightJoyStick;
@property (strong, nonatomic) IBOutlet UIPanGestureRecognizer *rightTouch;
@property (strong, nonatomic) IBOutlet UIPanGestureRecognizer *leftTouch;

- (void)sendDataString:(NSData *)data;
- (IBAction)leftTouchBegin:(id)sender;
- (IBAction)rightTouchBegins:(id)sender;

@end
