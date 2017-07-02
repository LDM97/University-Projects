
/* JQuery code to make the navigation bar stick to the browser viewport when scrolled past.
   Concept from YouTube video: https://www.youtube.com/watch?v=utonytGKodc */

jQuery( document ).ready( function () {
	
	var navOffset = jQuery( "#nav" ).offset().top;
	
	jQuery( "#nav" ).wrap( '<div class = "nav-placeholder"></div>' );
	jQuery( ".nav-placeholder" ).height( jQuery( "#nav" ).outerHeight() );
	
	jQuery( "#nav" ).wrapInner( '<div class = "nav-inner" </div>' );
	jQuery( ".nav-inner" ).wrapInner( '<div class = "nav-inner-most" </div>' );
	
	jQuery( window ).scroll( function () {
		var scrollPos = jQuery( window ).scrollTop();
		
		if( scrollPos >= navOffset ){
			jQuery( "#nav" ).addClass( "fixed" );
		} else {
			jQuery( "#nav" ).removeClass( "fixed" );
		}
	});
	
});