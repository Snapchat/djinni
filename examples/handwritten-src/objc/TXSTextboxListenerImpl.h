#import "TXSTextboxListener.h"
#import <Foundation/Foundation.h>
#import <UIKit/UITextView.h>

@interface TXSTextboxListenerImpl : NSObject <TXSTextboxListener>

- (id)initWithUITextView:(UITextView *)textView;

@end
