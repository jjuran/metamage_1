/*
	about_box_metrics.hh
	--------------------
*/

#ifndef MACAPP_ABOUTBOXMETRICS_HH
#define MACAPP_ABOUTBOXMETRICS_HH


namespace mac {
namespace app {

enum
{
	kAboutBoxIconEdgeLength       =  32 << TARGET_API_MAC_CARBON,
	
	kAboutBoxIconWidth            = kAboutBoxIconEdgeLength,
	kAboutBoxIconHorizontalMargin = 110,
	kAboutBoxTextHorizontalMargin =  24,
	
	kAboutBoxTopMargin            =   8,
	kAboutBoxIconHeight           = kAboutBoxIconEdgeLength,
	kAboutBoxIconToTextGap        =  12,
	kAboutBoxAppNameHeight        =  19,
	kAboutBoxInterTextGap         =   8,
	kAboutBoxDetailHeight         =  12,
	kAboutBoxBottomMargin         =  20,
	
	kAboutBoxWidth = kAboutBoxIconWidth
	               + kAboutBoxIconHorizontalMargin * 2,
	
	kAboutBoxHeight = kAboutBoxTopMargin
	                + kAboutBoxIconHeight
	                + kAboutBoxIconToTextGap
	                + kAboutBoxAppNameHeight
	                + kAboutBoxInterTextGap
	                + kAboutBoxDetailHeight
	                + kAboutBoxInterTextGap
	                + kAboutBoxDetailHeight
	                + kAboutBoxBottomMargin,
	
	kAboutBoxTextWidth = kAboutBoxWidth
	                   - kAboutBoxTextHorizontalMargin * 2,
};

}
}

#endif
