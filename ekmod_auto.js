/*var Drupal = Drupal || { 'settings': {}, 'behaviors': {}, 'themes': {}, 'locale': {} };
var settings = settings || Drupal.settings;
// Allow other JavaScript libraries to use $.
jQuery.noConflict();

*/
//var langu;
(function ($) {

// Check that ekmod library is available
if (typeof ekmod === 'undefined') return;

Drupal.behaviors.ekmod = {
  attach: function(context, settings) {
/*
	var image = {
            player: "img",
            title: "Tiger",
//          gallery : gall_id,
            gall_id : gall_id,
            content: "http://127.0.0.1/EDrupal/sites/default/files/styles/175-175/public/T2-2.jpg",
//          options: options
        };
*/
//	  if (settings.stringoverrides || false) {      // Make sure the strings array is initialized.      
//Drupal.locale.strings = Drupal.locale.strings || {};      // Add the string overrides translations to the strings array.     
// jQuery.extend(Drupal.locale.strings, settings.stringoverrides);    }
//	  	var translation = settings.ekmod.translation.loading;
//		var langu =  Drupal.t('Loading');
//		var dr = Drupal.locale.strings["Loading"];
/*    if (settings.ekmod.auto_enable_all_images == 1) {
      $("a[href$='jpg'], a[href$='png'], a[href$='gif'], a[href$='jpeg'], a[href$='bmp'], a[href$='JPG'], a[href$='PNG'], a[href$='GIF'], a[href$='JPEG'], a[href$='BMP']").each(function() {
        if ($(this).attr('rel') == '') {		
          if (settings.ekmod.auto_gallery == 1) {
          $(this).attr('rel', 'ekmod[gallery]');		
          }
          else {
            $(this).attr('rel', 'ekmod');		
          }
        }
      });
    }*/
      ekmod.setup();
/*      $('#accordion').accordion();		*/
/*      $('ul.menu_clearfix_2 li.expanded:not(.leaf)').click(function() {
		  //display an alert
//		  alert('Handler for .click() called.');
		  //show the element #container
		  $('.menu_clearfix_2 ul').slideToggle('slow');	
		  $(this).toggleClass('expanded');
		  return false;
//		  $('#container').show();
	  });	
*/
/*  	$('.menu_clearfix_2 li').click(function() {
		$(this).next().toggle('slow');
		return false;
	}).next().hide();*/
/*    	$('.menu_clearfix_2 li').click(function() {
    		$(this).next().toggle('slow');
    		return false;
    	}).next().hide();*/
/*      	$('.menu_clearfix_2').accordion();		*/
/*      $.getJSON("json_generate/node/1", function(json){
      console.log("the json", json);
      response = jQuery.parseJSON(response);
*/
/*
    ekmod.setup("a", {
       
 		gallery:            "My Movies",
        autoplayMovies:     true

    });
    ekmod.open([image]);
    ekmod.init();
*/
//	$(window).load(function(){  
//	$('#sb-title-inner').append('<a id="sb-nav-close" onclick="ekmod.close()" title="Close"></a> '); 
//	$('#sb-loading-inner span').html(Drupal.t('Loading'));
//	$('.sb-nav-close a').html(Drupal.t('Close'));
//	$('.sb-gallery-field_dnuotr').append('<a id="sb-nav-close" onclick="ekmod.close()" title="Close"></a> ');/*attr('width','220');*/
/*	$('a#sb-nav-next').click(function(e) {
        $('div#sb-title-2').text('1');
    });  */
//	return  land;
//	var land = settings.ekmod.langud;
//	return land;
    //for each description div...   
/*    $('#sb-loading-inner').each(function(){   
        //...set the opacity to 0...   
        $(this).css('opacity', 0);   
        //..set width same as the image...   
        $(this).css('width', $(this).siblings('img').width());   
        //...get the parent (the wrapper) and set it's width same as the image width... '   
        $(this).parent().css('width', $(this).siblings('img').width());   
        //...set the display to block   
        $(this).css('display', 'block');   
    });   
  
    $('#sb-loading-inner').hover(function(){   
        //when mouse hover over the wrapper div   
        //get it's children elements with class description '   
        //and show it using fadeTo   
        $(this).children('.description').stop().fadeTo(500, 0.7);   
    },function(){   
        //when mouse out of the wrapper div   
        //use fadeTo to hide the div   
        $(this).children('.description').stop().fadeTo(500, 0);   
    }); */
//	});  
  }
};
})(jQuery);