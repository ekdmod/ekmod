var Drupal = Drupal || { 'settings': {}, 'behaviors': {}, 'themes': {}, 'locale': {} };
var settings = settings || Drupal.settings;
// Allow other JavaScript libraries to use $.
jQuery.noConflict();
//var langu;
(function ($) {
Drupal.behaviors.ekmod = {
  attach: function(context, settings) {
	$("#edit-ct").change(function(index){$("#edit-category").empty();var cy = settings.ekmod_form.ct[this.value];$.each(settings.ekmod_form[cy[0]][cy[2]][cy[1]],function (index,optionData) {$('select#edit-category')[0].add(new Option(optionData,index));});});
	$("#edit-category").change(function(index){var cy = settings.ekmod_form.ct[$("select#edit-ct").val()];$('[name=category_hidden]:hidden').val(settings.ekmod_form[cy[0]][cy[2]][cy[1]][this.value]);});  
  }
};
})(jQuery);