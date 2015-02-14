//
//  ViewController.h
//  BLETest
//
//  Created by Robert Mannuzza on 1/9/15.
//  Copyright (c) 2015 Robert Mannuzza. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "BLEAdapter.h"

@interface ViewController : UIViewController < UITableViewDelegate, UITableViewDataSource >

@property (strong, nonatomic) BLEAdapter* bleWrapper;
@property (strong, nonatomic) CBPeripheral *selected;

@property (strong, nonatomic) IBOutlet UITableView *tbl_data;
@property (strong, nonatomic) BLEAdapter *bleAdapter;
@property (strong, nonatomic) CBPeripheral *toConnect;

- (IBAction)scan_click:(id)sender;

@end

