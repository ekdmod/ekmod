<?php

function node_ekmod_overview_type() {
  
  $types = ekmod_load_types()->types;
  $names = ekmod_load_types()->names;
  $field_ui = module_exists('field_ui');
  //$field_ui_enabled = field_ui_enabled()->status == 1 ? true : false;
  // enable_field_ui_module();
  //$header = array(t('Name'), array('data' => t('Operations'), 'colspan' => $field_ui ? '4' : '2'));  
  $rows = array();

  foreach ($names as $key => $name) {
      $type = $types[$key];
//    debug($type);
      $type_url_str = str_replace('_', '-', $type->type);
      $row = array(theme('ekmod', array('name' => $name, 'type' => $type)));
      // Set the edit column.
      $row[] = array('data' => l(t('edit'), 'admin/structure/types/ekmod/content_types/manage/' . $type_url_str));

      if ($field_ui) {
        // Manage fields.
        $row[] = array('data' => l(t('manage fields'), 'admin/structure/types/ekmod/content_types/manage/' . $type_url_str . '/fields'));

        // Display fields.
        $row[] = array('data' => l(t('manage display'), 'admin/structure/types/ekmod/content_types/manage/' . $type_url_str . '/display'));
      }

      // Set the delete column.
      if ($type->delete) {
        $row[] = array('data' => l(t('delete'), 'admin/structure/types/ekmod/content_types/manage/' . $type_url_str . '/delete'));
        $colspan = 4;
      }
      else {
        //$row[] = array('data' => '');
        $colspan = 3;
      }

      $rows[] = $row;
  }
  $build['ekmod_table'] = array(
    '#theme' => 'table',
    '#header' => array(t('Name'), array('data' => t('Operations'), 'colspan' => $colspan)),
    '#rows' => $rows,
    '#empty' => t('No content types available. <a href="@link">Add content type</a>.', array('@link' => url('admin/structure/types/ekmod/add'))),
  );

  return $build;
}

/*Manage EKMOD content types*/
function ekmod_type_arg_form($form, &$form_state, $type = NULL) {

  $form['#ekmod_type'] = $type;
  $form['name'] = array(
	'#title' => t('Name'),
	'#type' => 'textfield',
	'#default_value' => $type->name,
	'#description' => t('The human-readable name of this content type. This text will be displayed as part of the list on the <em>Add new content</em> page. It is recommended that this name begin with a capital letter and contain only letters, numbers, and spaces. This name must be unique.'),
	'#required' => TRUE,
	'#size' => 30,
  	'#disabled' => !$type->delete,
  );
	
  $form['type'] = array(
    '#type' => 'machine_name',
    '#default_value' => $type->type,
    '#maxlength' => 32,
    '#disabled' => $type->locked,
    '#machine_name' => array(
      'exists' => 'ekmod_type_name_ld',
    ),
    '#description' => t('A unique machine-readable name for this content type. It must only contain lowercase letters, numbers, and underscores. This name will be used for constructing the URL of the %node-add page, in which underscores will be converted into hyphens.', array(
      '%node-add' => t('Add new content'),
    )),
    '#disabled' => !$type->delete,
  );
  
  $form['description'] = array(
	'#title' => t('Description'),
	'#type' => 'textarea',
	'#default_value' => $type->description,
	'#description' => t('Describe this content type. The text will be displayed on the <em>Add new content</em> page.'),
  	'#disabled' => !$type->delete,
  );
  $form['additional_settings'] = array(
    '#type' => 'vertical_tabs',
    '#attached' => array(
  		'js' => array(drupal_get_path('module','ekmod') . '/content_types.js'),
  	),
  );
  $form['submission'] = array(
  	'#type' => 'fieldset',
  	'#title' => t('Submission form settings'),
  	'#collapsible' => TRUE,
  	'#group' => 'additional_settings', //#!
  );
  $form['submission']['title_label'] = array(
  	'#title' => t('Title field label'),
  	'#type' => 'textfield',
  	'#default_value' => $type->title_label,
  	'#required' => TRUE,
  );
    if (!$type->has_title) {
    // Avoid overwriting a content type that intentionally does not have a
    // title field.
    $form['submission']['title_label']['#attributes'] = array('disabled' => 'disabled');
    $form['submission']['title_label']['#description'] = t('This content type does not have a title field.');
    $form['submission']['title_label']['#required'] = FALSE;
  }

  $form['submission']['node_preview'] = array(
    '#type' => 'radios',
    '#title' => t('Preview before submitting'),
    '#default_value' => variable_get('ekmod_preview_' . $type->type, DRUPAL_OPTIONAL),
    '#options' => array(
      DRUPAL_DISABLED => t('Disabled'),
      DRUPAL_OPTIONAL => t('Optional'),
      DRUPAL_REQUIRED => t('Required'),
    ),
  );
  $form['submission']['help']  = array(
    '#type' => 'textarea',
    '#title' => t('Explanation or submission guidelines'),
    '#default_value' => $type->help,
    '#description' => t('This text will be displayed at the top of the page when creating or editing content of this type.'),
  );
  $form['workflow'] = array(
    '#type' => 'fieldset',
    '#title' => t('Publishing options'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#group' => 'additional_settings',
  );
  $form['workflow']['ekmod_options'] = array('#type' => 'checkboxes',
    '#title' => t('Default options'),
    '#default_value' => variable_get('ekmod_options_' . $type->type, array('status', 'promote')),
    '#options' => array(
      'status' => t('Published'),
      'promote' => t('Promoted to front page'),
      'sticky' => t('Sticky at top of lists'),
      'revision' => t('Create new revision'),
    ),
    '#description' => t('Users with the <em>Administer content</em> permission will be able to override these options.'),
  );
  $form['display'] = array(
    '#type' => 'fieldset',
    '#title' => t('Display settings'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#group' => 'additional_settings',
  );
  $form['display']['ekmod_submitted'] = array(
    '#type' => 'checkbox',
    '#title' => t('Display author and date information.'),
    '#default_value' => variable_get('ekmod_submitted_' . $type->type, TRUE),
    '#description' => t('Author username and publish date will be displayed.'),
  );
  $form['old_type'] = array(
    '#type' => 'value',
    '#value' => $type->type,
  );
  $form['orig_type'] = array(
    '#type' => 'value',
    '#value' => isset($type->orig_type) ? $type->orig_type : '',
  );
  $form['base'] = array(
    '#type' => 'value',
    '#value' => $type->base,
  );
  $form['custom'] = array(
    '#type' => 'value',
    '#value' => $type->custom,
  );
  $form['modified'] = array(
    '#type' => 'value',
    '#value' => $type->modified,
  );
  $form['locked'] = array(
    '#type' => 'value',
    '#value' => $type->locked,
  );

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save content type'),
    '#weight' => 40,
  );

  if ($type->custom) {
    if (!empty($type->type)) {
      $form['actions']['delete'] = array(
        '#type' => 'submit',
        '#value' => t('Delete content type'),
        '#weight' => 45,
      );
    }
  }
  
	$form['#submit'] = array('manage_ekmod_types_submit');
	$form['#theme'] = 'ekmod_types_managed';
	return $form;
}
/**
 * Form validation handler for node_type_form().
 *
 * @see node_type_form_submit()
 */
function manage_ekmod_types_validate($form, &$form_state) {
  $type = new stdClass();
  $type->type = trim($form_state['values']['type']);
  $type->name = trim($form_state['values']['name']);

  // Work out what the type was before the user submitted this form
  $old_type = trim($form_state['values']['old_type']);

  $types = node_type_get_names();

  if (!$form_state['values']['locked']) {
    // 'theme' conflicts with theme_node_form().
    // '0' is invalid, since elsewhere we check it using empty().
    if (in_array($type->type, array('0', 'theme'))) {
      form_set_error('type', t("Invalid machine-readable name. Enter a name other than %invalid.", array('%invalid' => $type->type)));
    }
  }

  $names = array_flip($types);

  if (isset($names[$type->name]) && $names[$type->name] != $old_type) {
    form_set_error('name', t('The human-readable name %name is already taken.', array('%name' => $type->name)));
  }
}

function ekmod_type_delete_confirm() {

}
/**
 * @file
 * Administrative interface for the EKMOD module.
 */
/**
 * Displays the content type admin overview page.
 */
function ekmod_structure_overview($form, &$form_state) {
  $form['content'] = array (
  '#type' => 'textfield',
  '#title' => t('C'),
  
  );
  $form['ekmod_content'] = array(

  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit'),
  );

return $form;
}

function ekmod_overview_type() {
  
  $types = ekmod_load_types()->types;
  $names = ekmod_load_types()->names;
  
  $field_ui = module_exists('field_ui');
  $header = array(t('Name'), array('data' => t('Operations'), 'colspan' => $field_ui ? '4' : '2'));
  $rows = array();

  foreach ($names as $key => $name) {
      $type = $types[$key];
//    debug($type);
      $type_url_str = str_replace('_', '-', $type->type);
      $row = array(theme('ekmod', array('name' => $name, 'type' => $type)));
      // Set the edit column.
      $row[] = array('data' => l(t('edit'), 'admin/structure/ekmod/content_types/manage/' . $type_url_str));

      if ($field_ui) {
        // Manage fields.
        $row[] = array('data' => l(t('manage fields'), 'admin/structure/ekmod/content_types/manage/' . $type_url_str . '/fields'));

        // Display fields.
        $row[] = array('data' => l(t('manage display'), 'admin/structure/ekmod/content_types/manage/' . $type_url_str . '/display'));
      }

      // Set the delete column.
      if ($type->delete) {
        $row[] = array('data' => l(t('delete'), 'admin/structure/ekmod/content_types/manage/' . $type_url_str . '/delete'));
      }
      else {
        $row[] = array('data' => '');
      }

      $rows[] = $row;
  }

  $build['ekmod_table'] = array(
    '#theme' => 'table',
    '#header' => $header,
    '#rows' => $rows,
    '#empty' => t('No content types available. <a href="@link">Add content type</a>.', array('@link' => url('admin/structure/ekmod/content_types'))),
  );

  return $build;
}

function ekmod_permissions_form($form, $form_state) {
//$hide_descriptions = system_admin_compact_mode();
//$session = isset($_SESSION['ekmod_perm_filter']) ? $_SESSION['node_perm_filter'] : array();
  $form['filter'] = ekmod_filter_form();
  $form['#submit'][] = 'ekmod_filter_form_submit';
  $form['perm'] = ekmod_perm();
//$ekmod_perm = db_query('SELECT id FROM ekmod_module');
}
/**
 * FAPI definition for the EKMOD global settings form.
 *
 * @ingroup forms
 * @see ekmod_global_settings_validate()
 */
function ekmod_global_settings() {

  drupal_add_library('ekmod', 'ekmod');
  drupal_add_css(drupal_get_path('module', 'ekmod') . '/ekmod_admin.css');

  $ekmod_path = EKMOD_DEFAULT_PATH;
	
  if (!variable_get('ekmod_enabled', TRUE)) {
	  // Enable warning
	  $form['ekmod']['enable_test'] = array(
	    '#prefix' => '<div class="messages warning">',
	    '#markup' => t('EKMOD is disabled.'),
	    '#suffix' => '</div>',
	  );
  }

  // Test
  $form['ekmod']['test'] = array(
    '#prefix' => '<div id="ekmod-settings-test">',
    '#markup' => l(t('Test EKMOD with the current settings.'), 'modules/image/sample.png'/*,array('attributes' => array('rel' => 'ekmod'))*/),
    '#suffix' => '</div>',
  );
  $vmk['default'] = t('Default');
  $vm = ekmod_entity_info();
  foreach($vm['ekmod']['view modes'] as $k => $v) {
  if($v['custom settings'] == TRUE) 
  {
  $vmk[$k] = $v['label'] ; 
  }
  }
  // Global settings
  $form['ekmod']['ekmod_enabled'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable EKMOD'),
    '#default_value' => variable_get('ekmod_enabled', TRUE),
    '#description' => t('Check this box to enable EKMOD for the entire site.'),
  );
  $form['ekmod']['ekmod_location'] = array(
    '#type' => 'textfield',
    '#title' => t('EKMOD location'),
    '#default_value' => variable_get('ekmod_location', $ekmod_path),
    '#description' => t('Enter the location of the !download. It is recommended to use e.g. sites/all/libraries/ekmod.', array('!download' => l(t('third party EKMOD distribution'), 'http://www.ekmod.com/', array('absolute' => TRUE)))),
  );
  $form['ekmod']['ekmod_time_2'] = array(
    '#type' => 'textfield',
    '#title' => t('EKMOD gallery updater'),
    '#description' => t('Update gallery content in minutes.'),
    '#size' => 5,
    '#maxlength' => 4,
    '#default_value' => variable_get('ekmod_time_2', 10),
  );
  $form['ekmod']['activation'] = array(
    '#type' => 'fieldset',
    '#title' => t('Page specific activation settings'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $access = user_access('use PHP for settings');
  $activation = variable_get('ekmod_activation_type', EKMOD_ACTIVATION_NOTLISTED);
  $pages = variable_get('ekmod_pages', "admin*\nimg_assist*\nnode/add/*\nnode/*/edit");
  if ($activation == EKMOD_ACTIVATION_PHP && !$access) {
    $form['ekmod']['activation']['ekmod_activation_type'] = array(
      '#type' => 'value',
      '#value' => EKMOD_ACTIVATION_PHP,
    );
    $form['ekmod']['activation']['ekmod_pages'] = array(
      '#type' => 'value',
      '#value' => $pages,
    );
  }
  else {
    $options = array(
      EKMOD_ACTIVATION_NOTLISTED => t('Enable on every page except the listed pages.'),
      EKMOD_ACTIVATION_LISTED => t('Enable on only the listed pages.'),
    );
    $description = t("Specify pages by using their paths. Enter one path per line. The '*' character is a wildcard. Example paths are %blog for the blog page and %blog-wildcard for every personal blog. %front is the front page.", array('%blog' => 'blog', '%blog-wildcard' => 'blog/*', '%front' => '<front>'));

    if (module_exists('php') && $access) {
      $options += array(EKMOD_ACTIVATION_PHP => t('Enable if the following PHP code returns <code>TRUE</code> (PHP-mode, experts only).'));
      $title = t('Pages or PHP code');
      $description .= ' ' . t('If the PHP option is chosen, enter PHP code between %php. Note that executing incorrect PHP code can break your Drupal site.', array('%php' => '<?php ?>'));
    }
    else {
      $title = t('Pages');
    }
    $form['ekmod']['activation']['ekmod_activation_type'] = array(
      '#type' => 'radios',
      '#title' => t('Enable EKMOD on specific pages'),
      '#options' => $options,
      '#default_value' => $activation,
    );
    $form['ekmod']['activation']['ekmod_pages'] = array(
      '#type' => 'textarea',
      '#title' => '<span class="element-invisible">' . $title . '</span>',
      '#default_value' => $pages,
      '#description' => $description,
    );
    $form['ekmod']['activation']['ekmod_pager_settings'] = array(
    '#type' => 'textfield',
    '#title' => t('Element pager'),
    '#description' => t('Show quantinty of element in a page .'),
    '#size' => 5,
    '#maxlength' => 4,
    '#default_value' => variable_get('ekmod_pager_settings', TRUE),
  );
  }

  // Options
  $form['ekmod']['ekmod_options'] = array(
    '#type' => 'fieldset',
    '#title' => t('EKMOD options'),
    '#description' => t('EKMOD is highly configurable, but can also be used with little to no configuration at all. The following options may be used to configure EKMOD on a site-wide basis.'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  // Animation
  $form['ekmod']['ekmod_options']['animation_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Animation'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['ekmod']['ekmod_options']['animation_settings']['ekmod_animate'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable animation'),
    '#description' => t('Set this false to disable all fancy animations (except fades). This can improve the overall effect on computers with poor performance.'),
    '#default_value' => variable_get('ekmod_animate', TRUE),
  );
  $form['ekmod']['ekmod_options']['animation_settings']['ekmod_animate_fade'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable fading animations'),
    '#description' => t('Set this false to disable all fading animations.'),
    '#default_value' => variable_get('ekmod_animate_fade', TRUE),
  );
  $form['ekmod']['ekmod_options']['animation_settings']['ekmod_animation_sequence'] = array(
    '#type' => 'select',
    '#title' => t('Animation sequence'),
    '#multiple' => FALSE,
    '#description' => t('The animation sequence to use when resizing EKMOD.'),
    '#options' => array(
      'wh' => t('Width then height'),
      'hw' => t('Height then width'),
      'sync' => t('Simultaneously'),
    ),
    '#default_value' => variable_get('ekmod_animation_sequence', 'wh'),
  );
  $form['ekmod']['ekmod_options']['animation_settings']['ekmod_resize_duration'] = array(
    '#type' => 'textfield',
    '#title' => t('Resize duration'),
    '#description' => t('The duration (in seconds) of the resizing animations.'),
    '#size' => 5,
    '#maxlength' => 4,
    '#default_value' => variable_get('ekmod_resize_duration', 0.35),
  );
  $form['ekmod']['ekmod_options']['animation_settings']['ekmod_fade_duration'] = array(
    '#type' => 'textfield',
    '#title' => t('Fade duration'),
    '#description' => t('The duration (in seconds) of the overlay fade animation.'),
    '#size' => 5,
    '#maxlength' => 4,
    '#default_value' => variable_get('ekmod_fade_duration', 0.35),
  );
  $form['ekmod']['ekmod_options']['animation_settings']['ekmod_initial_height'] = array(
    '#type' => 'textfield',
    '#title' => t('Initial height'),
    '#description' => t('The height of EKMOD (in pixels) when it first appears on the screen.'),
    '#size' => 5,
    '#maxlength' => 4,
    '#default_value' => variable_get('ekmod_initial_height', 160),
  );
  $form['ekmod']['ekmod_options']['animation_settings']['ekmod_initial_width'] = array(
    '#type' => 'textfield',
    '#title' => t('Initial width'),
    '#description' => t('The width of EKMOD (in pixels) when it first appears on the screen.'),
    '#size' => 5,
    '#maxlength' => 4,
    '#default_value' => variable_get('ekmod_initial_width', 320),
  );

  // Overlay
  $form['ekmod']['ekmod_options']['overlay'] = array(
    '#type' => 'fieldset',
    '#title' => t('Overlay'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['ekmod']['ekmod_options']['overlay']['ekmod_overlay_color'] = array(
    '#type' => 'textfield',
    '#title' => t('Overlay color'),
    '#description' => t('Select a hexadecimal color value for the overlay (e.g. 000 or 000000 for black).'),
    '#size' => 8,
    '#maxlength' => 7,
    '#default_value' => variable_get('ekmod_overlay_color', '000'),
    '#field_prefix' => '#',
  );
  $form['ekmod']['ekmod_options']['overlay']['ekmod_overlay_opacity'] = array(
    '#type' => 'textfield',
    '#title' => t('Overlay opacity'),
    '#description' => t('The opacity of the overlay. Accepts values between 0 and 1. 0 is fully transparent, 1 is fully opaque.'),
    '#size' => 5,
    '#maxlength' => 4,
    '#default_value' => variable_get('ekmod_overlay_opacity', 0.85),
  );
  $form['ekmod']['ekmod_options']['overlay']['ekmod_viewport_padding'] = array(
    '#type' => 'textfield',
    '#title' => t('Viewport padding'),
    '#description' => t('The amount of padding (in pixels) to maintain around the edge of the browser window.'),
    '#size' => 4,
    '#maxlength' => 4,
    '#default_value' => variable_get('ekmod_viewport_padding', 20),
  );

  // Galleries
  $form['ekmod']['ekmod_options']['gallery'] = array(
    '#type' => 'fieldset',
    '#title' => t('Galleries'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['ekmod']['ekmod_options']['gallery']['ekmod_display_nav'] = array(
    '#type' => 'checkbox',
    '#title' => t('Display gallery navigation.'),
    '#description' => t('Enable display of the gallery navigation controls.'),
    '#default_value' => variable_get('ekmod_display_nav', TRUE),
  );
  $form['ekmod']['ekmod_options']['gallery']['ekmod_continuous_galleries'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable continuous galleries'),
    '#description' => t('By default, the galleries will not let a user go before the first image or after the last. Enabling this feature will let the user go directly to the first image in a gallery from the last one by selecting "Next".'),
    '#default_value' => variable_get('ekmod_continuous_galleries', FALSE),
  );
  $form['ekmod']['ekmod_options']['gallery']['ekmod_display_counter'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable counter'),
    '#description' => t('Enable display of the gallery counter. Counters are never displayed on elements that are not part of a gallery.'),
    '#default_value' => variable_get('ekmod_display_counter', TRUE),
  );
  $form['ekmod']['ekmod_options']['gallery']['ekmod_counter_type'] = array(
    '#type' => 'select',
    '#title' => t('Counter type'),
    '#multiple' => FALSE,
    '#description' => t('The mode to use for the gallery counter. May be either \'default\' or \'skip\'. The default counter is a simple \'1 of 5\' message. The skip counter displays a separate link to each piece in the gallery, enabling quick navigation in large galleries.'),
    '#options' => array(
      'default' => t('Default'),
      'skip' => t('Skip'),
    ),
    '#default_value' => variable_get('ekmod_counter_type', 'default'),
  );
  $form['ekmod']['ekmod_options']['gallery']['ekmod_counter_limit'] = array(
    '#type' => 'textfield',
    '#title' => t('Counter limit'),
    '#description' => t('Limits the number of counter links that will be displayed in a "skip" style counter. If the actual number of gallery elements is greater than this value, the counter will be restrained to the elements immediately preceding and following the current element.'),
    '#default_value' => variable_get('ekmod_counter_limit', 10),
  );
  $form['ekmod']['ekmod_options']['gallery']['ekmod_slideshow_delay'] = array(
    '#type' => 'textfield',
    '#title' => t('Slideshow delay'),
    '#description' => t('A delay (in seconds) to use for slideshows. If set to anything other than 0, this value determines an interval at which EKMOD will automatically proceed to the next piece in the gallery.'),
    '#default_value' => variable_get('ekmod_slideshow_delay', 0),
  );

  // Movies
  $form['ekmod']['ekmod_options']['movies'] = array(
    '#type' => 'fieldset',
    '#title' => t('Movies'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['ekmod']['ekmod_options']['movies']['ekmod_autoplay_movies'] = array(
    '#type' => 'checkbox',
    '#title' => t('Auto-play movies'),
    '#description' => t('Automatically play movies when they are loaded.'),
    '#default_value' => variable_get('ekmod_autoplay_movies', 1),
  );
  $form['ekmod']['ekmod_options']['movies']['ekmod_show_movie_controls'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable movie controls'),
    '#description' => t('Enable display of QuickTime and Windows Media player movie controls.'),
    '#default_value' => variable_get('ekmod_show_movie_controls', TRUE),
  );

  // Input controls
  $form['ekmod']['ekmod_options']['input'] = array(
    '#type' => 'fieldset',
    '#title' => t('Input controls'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['ekmod']['ekmod_options']['input']['ekmod_overlay_listen'] = array(
    '#type' => 'checkbox',
    '#title' => t('Mouse click closes overlay.'),
    '#description' => t('Enable listening for mouse clicks on the overlay that will close EKMOD.'),
    '#default_value' => variable_get('ekmod_overlay_listen', TRUE),
  );
  $form['ekmod']['ekmod_options']['input']['ekmod_enable_keys'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable keys'),
    '#description' => t('Enable keyboard navigation of galleries.'),
    '#default_value' => variable_get('ekmod_enable_keys', TRUE),
  );

  // Media handling
  $form['ekmod']['ekmod_options']['media_handling'] = array(
    '#type' => 'fieldset',
    '#title' => t('Media handling'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['ekmod']['ekmod_options']['media_handling']['ekmod_handle_oversize'] = array(
    '#type' => 'select',
    '#title' => t('Handle large images'),
    '#multiple' => FALSE,
    '#description' => t('The mode to use for handling images that are too large for the viewport. The "none" setting will not alter the image dimensions, though clipping may occur. Setting this to "resize" enables on-the-fly resizing of large content. In this mode, the height and width of large, resizable content will be adjusted so that it may still be viewed in its entirety while maintaining its original aspect ratio. The "drag" mode will display an oversized image at its original resolution, but will allow the user to drag it within the view to see portions that may be clipped.'),
    '#options' => array(
      'resize' => t('Resize'),
      'drag' => t('Drag'),
      'none' => t('None'),
    ),
    '#default_value' => variable_get('ekmod_handle_oversize', 'resize'),
  );
  $form['ekmod']['ekmod_options']['media_handling']['ekmod_handle_unsupported'] = array(
    '#type' => 'select',
    '#title' => t('Handle unsupported'),
    '#multiple' => FALSE,
    '#description' => t('The mode to use for handling unsupported media. May be either <strong>link</strong> or <strong>remove</strong>. Media are unsupported when the browser plugin required to display the media properly is not installed. The link option will display a user-friendly error message with a link to a page where the needed plugin can be downloaded. The remove option will simply remove any unsupported gallery elements from the gallery before displaying it. With this option, if the element is not part of a gallery, the link will simply be followed.'),
    '#options' => array(
      'link' => t('Link'),
      'remove' => t('Remove'),
    ),
    '#default_value' => variable_get('ekmod_handle_unsupported', 'link'),
  );

  // Libraries
  $form['ekmod']['ekmod_options']['libraries'] = array(
    '#type' => 'fieldset',
    '#title' => t('Libraries'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['ekmod']['ekmod_options']['libraries']['ekmod_use_sizzle'] = array(
    '#type' => 'checkbox',
    '#title' => t('Use Sizzle'),
    '#description' => t('Enable loading the Sizzle.js CSS selector library.'),
    '#default_value' => variable_get('ekmod_use_sizzle', FALSE),
  );
  // view modes 
  $form['ekmod']['ekmod_options']['view_modes'] = array(
    '#type' => 'fieldset',
    '#title' => t('View modes'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['ekmod']['ekmod_options']['view_modes']['ekmod_galler_view_mode'] = array(
    '#type' => 'select',
    '#title' => t('Gallery view mode'),
    '#multiple' => FALSE,
    '#description' => t('View mode to display gallery content.'),
/*
    '#options' => array(
      'full' => t('full'),
      'print' => t('print'),
      'default' => t('default'),
    ),
*/
  	'#options' => $vmk,
    '#default_value' => variable_get('ekmod_galler_view_mode', 'default'),
  );
    $form['ekmod']['ekmod_options']['view_modes']['ekmod_category_view_mode'] = array(
    '#type' => 'select',
    '#title' => t('Category\'s view mode'),
    '#multiple' => FALSE,
    '#description' => t('View mode to display gallery content.'),
    '#options' => $vmk,
/*
    '#options' => array(
      'full' => t('full'),
      'print' => t('print'),
      'default' => t('default'),
    ),
*/
    '#default_value' => variable_get('ekmod_category_view_mode', 'default'),
  );
  return system_settings_form($form);
}

/**
 * Validate admin settings form inputs.
 */
function ekmod_global_settings_validate($form, &$form_state) {
  $ekmod_timer = $form_state['values']['ekmod_time_2'];
  $ekmod_page = $form_state['values']['ekmod_pager_settings'];
  $color = $form_state['values']['ekmod_overlay_color'];
  $opacity = $form_state['values']['ekmod_overlay_opacity'];
  $slideshow_delay = $form_state['values']['ekmod_slideshow_delay'];
  $resize_duration = $form_state['values']['ekmod_resize_duration'];
  $fade_duration = $form_state['values']['ekmod_fade_duration'];
  $viewport_padding = $form_state['values']['ekmod_viewport_padding'];
  $initial_height = $form_state['values']['ekmod_initial_height'];
  $initial_width = $form_state['values']['ekmod_initial_width'];
  $counter_limit = $form_state['values']['ekmod_counter_limit'];
  
  if (!is_numeric($ekmod_page) || $ekmod_page < 0 ) {
    form_set_error('ekmod_pager_settings', t('You must enter a number.'));
  }
  if (!is_numeric($ekmod_timer) || $ekmod_timer < 0 ) {
    form_set_error('ekmod_time_2', t('You must enter a number.'));
  }  
  if (!is_numeric($resize_duration) || $resize_duration < 0 || $resize_duration > 10) {
    form_set_error('ekmod_resize_duration', t('You must enter a number between 0 and 10.'));
  }

  if (!is_numeric($fade_duration) || $fade_duration < 0 || $fade_duration > 10) {
    form_set_error('ekmod_fade_duration', t('You must enter a number between 0 and 10.'));
  }

  if (!is_numeric($initial_height)) {
    form_set_error('ekmod_initial_height', t('You must enter a number.'));
  }
  else {
    $form_state['values']['ekmod_initial_height'] = floor($initial_height);
  }

  if (!is_numeric($initial_width)) {
    form_set_error('ekmod_initial_width', t('You must enter a number.'));
  }
  else {
    $form_state['values']['ekmod_initial_width'] = floor($initial_width);
  }

  if (!_validate_hex_color($color)) {
    form_set_error('ekmod_overlay_color', t('You must enter a properly formed hex value (e.g. 000 or 000000 for black.)'));
  }

  if ($opacity == '' || (floor($opacity) != 0 && $opacity != 1)) {
    form_set_error('ekmod_overlay_opacity', t('You must enter a decimal number between 0 and 1.'));
  }

  if (!is_numeric($viewport_padding) || $viewport_padding < 0 || $viewport_padding > 200) {
    form_set_error('ekmod_viewport_padding', t('You must enter a number between 0 and 200.'));
  }
  else {
    $form_state['values']['ekmod_viewport_padding'] = (int)$viewport_padding;
  }
}

/**
 * FAPI definition for the EKMOD automatic image handling settings form.
 *
 * @ingroup forms
 * @see ekmod_global_settings_validate()
 */
function ekmod_automatic_settings() {
  $form['ekmod_auto_enable_all_images'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable for all image links'),
    '#description' => t('Enable this option to automatically add the <code>rel="ekmod"</code> attribute to all links pointing to an image file.'),
    '#default_value' => variable_get('ekmod_auto_enable_all_images', 0),
  );
  $form['ekmod_auto_gallery'] = array(
    '#type' => 'checkbox',
    '#title' => t('Group images as a ekmod gallery'),
    '#description' => t('Enable this options to open all images in a EKMOD gallery rather than individually.'),
    '#default_value' => variable_get('ekmod_auto_gallery', 0),
  );
  $form['ekmod_enable_globally'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable EKMOD globally'),
    '#description' => t('Add EKMOD library on all nodes.'),
    '#default_value' => variable_get('ekmod_enable_globally', TRUE),
  );

  return system_settings_form($form);
}

/**
 * Implements hook_node_operations().
 */
function ekmod_operations() {
  $operations = array(
    'publish' => array(
      'label' => t('Publish selected content'),
      'callback' => 'ekmod_mass_update',
      'callback arguments' => array('updates' => array('status' => NODE_PUBLISHED)),
    ),
    'unpublish' => array(
      'label' => t('Unpublish selected content'),
      'callback' => 'ekmod_mass_update',
      'callback arguments' => array('updates' => array('status' => NODE_NOT_PUBLISHED)),
    ),
    'promote' => array(
      'label' => t('Promote selected content to front page'),
      'callback' => 'ekmod_mass_update',
      'callback arguments' => array('updates' => array('status' => NODE_PUBLISHED, 'promote' => NODE_PROMOTED)),
    ),
    'demote' => array(
      'label' => t('Demote selected content from front page'),
      'callback' => 'ekmod_mass_update',
      'callback arguments' => array('updates' => array('promote' => NODE_NOT_PROMOTED)),
    ),
    'sticky' => array(
      'label' => t('Make selected content sticky'),
      'callback' => 'ekmod_mass_update',
      'callback arguments' => array('updates' => array('status' => NODE_PUBLISHED, 'sticky' => NODE_STICKY)),
    ),
    'unsticky' => array(
      'label' => t('Make selected content not sticky'),
      'callback' => 'ekmod_mass_update',
      'callback arguments' => array('updates' => array('sticky' => NODE_NOT_STICKY)),
    ),
    'delete' => array(
      'label' => t('Delete selected content'),
      'callback' => NULL,
    ),
  );
  return $operations;
}
/**
 * List node administration filters that can be applied.
 */
function ekmod_filters() {
  // Regular filters
  $filters['status'] = array(
    'title' => t('status'),
    'options' => array(
      '[any]' => t('any'),
      'status-1' => t('published'),
      'status-0' => t('not published'),
      'promote-1' => t('promoted'),
      'promote-0' => t('not promoted'),
      'sticky-1' => t('sticky'),
      'sticky-0' => t('not sticky'),
    ),
  );
  // Include translation states if we have this module enabled
  if (module_exists('translation')) {
    $filters['status']['options'] += array(
      'translate-0' => t('Up to date translation'),
      'translate-1' => t('Outdated translation'),
    );
  }

  $filters['type'] = array(
    'title' => t('type'),
    'options' => array(
      '[any]' => t('any'),
    ) + /*+ node_type_get_names(), */ ekmod_load_types()->names,
  );
//debug(node_type_get_names());
  // Language filter if there is a list of languages
  if ($languages = module_invoke('locale', 'language_list')) {
    $languages = array(LANGUAGE_NONE => t('Language neutral')) + $languages;
    $filters['language'] = array(
      'title' => t('language'),
      'options' => array(
        '[any]' => t('any'),
      ) + $languages,
    );
  }
  return $filters;
}
/**
 * Return form for EKMOD administration filters.
 */
function ekmod_filter_form() {
  $session = isset($_SESSION['ekmod_overview_filter']) ? $_SESSION['ekmod_overview_filter'] : array();
  $filters = ekmod_filters();

  $i = 0;
  $form['filters'] = array(
    '#type' => 'fieldset',
    '#title' => t('Show only items where'),
    '#theme' => 'exposed_filters__node',
  );
  foreach ($session as $filter) {
    list($type, $value) = $filter;
    if ($type == 'term') {
      // Load term name from DB rather than search and parse options array.
      $value = module_invoke('taxonomy', 'term_load', $value);
      $value = $value->name;
    }
    elseif ($type == 'language') {
      $value = $value == LANGUAGE_NONE ? t('Language neutral') : module_invoke('locale', 'language_name', $value);
    }
    else {
      $value = $filters[$type]['options'][$value];
    }
    $t_args = array('%property' => $filters[$type]['title'], '%value' => $value);
    if ($i++) {
      $form['filters']['current'][] = array('#markup' => t('and where %property is %value', $t_args));
    }
    else {
      $form['filters']['current'][] = array('#markup' => t('where %property is %value', $t_args));
    }
    if (in_array($type, array('type', 'language'))) {
      // Remove the option if it is already being filtered on.
      unset($filters[$type]);
    }
  }

  $form['filters']['status'] = array(
    '#type' => 'container',
    '#attributes' => array('class' => array('clearfix')),
    '#prefix' => ($i ? '<div class="additional-filters">' . t('and where') . '</div>' : ''),
  );
  $form['filters']['status']['filters'] = array(
    '#type' => 'container',
    '#attributes' => array('class' => array('filters')),
  );
  foreach ($filters as $key => $filter) {
    $form['filters']['status']['filters'][$key] = array(
      '#type' => 'select',
      '#options' => $filter['options'],
      '#title' => $filter['title'],
      '#default_value' => '[any]',
    );
  }

  $form['filters']['status']['actions'] = array(
    '#type' => 'actions',
    '#attributes' => array('class' => array('container-inline')),
  );
  $form['filters']['status']['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => count($session) ? t('Refine') : t('Filter'),
  );
  if (count($session)) {
    $form['filters']['status']['actions']['undo'] = array('#type' => 'submit', '#value' => t('Undo'));
    $form['filters']['status']['actions']['reset'] = array('#type' => 'submit', '#value' => t('Reset'));
  }

  drupal_add_js('misc/form.js');

  return $form;
}

/**
 * Process result from node administration filter form.
 */
function ekmod_filter_form_submit($form, &$form_state) {
  $filters = ekmod_filters();
  switch ($form_state['values']['op']) {
    case t('Filter'):
    case t('Refine'):
      // Apply every filter that has a choice selected other than 'any'.
      foreach ($filters as $filter => $options) {
        if (isset($form_state['values'][$filter]) && $form_state['values'][$filter] != '[any]') {
          // Flatten the options array to accommodate hierarchical/nested options.
          $flat_options = form_options_flatten($filters[$filter]['options']);
          // Only accept valid selections offered on the dropdown, block bad input.
          if (isset($flat_options[$form_state['values'][$filter]])) {
            $_SESSION['ekmod_overview_filter'][] = array($filter, $form_state['values'][$filter]);
          }
        }
      }
      break;
    case t('Undo'):
      array_pop($_SESSION['ekmod_overview_filter']);
      break;
    case t('Reset'):
      $_SESSION['ekmod_overview_filter'] = array();
      break;
  }
}

function admin_content_ekmod($form, $form_state) { // ekmod_admin_content
  if (isset($form_state['values']['operation']) && $form_state['values']['operation'] == 'delete') {
    return ekmod_multiple_delete_confirm($form, $form_state, array_filter($form_state['values']['nodes']));
  }
  $form['filter'] = ekmod_filter_form();
  $form['#submit'][] = 'ekmod_filter_form_submit';
  $form['admin'] = ekmod_admin_nodes();

  return $form;
}
/**
 * Form builder: Builds the node administration overview.
 */
function ekmod_admin_nodes() {
  $ekmod_access = user_access('administer ekmod');
//  $admin_access = user_access('administer nodes');
  
  // Build the 'Update options' form.
/*
  $form['options'] = array(
    '#type' => 'fieldset',
    '#title' => t('Update options'),
    '#attributes' => array('class' => array('container-inline')),
    '#access' => $admin_access,
  );
*/
  $form['options'] = array(
    '#type' => 'fieldset',
    '#title' => t('Update options'),
    '#attributes' => array('class' => array('container-inline')),
    '#access' => $ekmod_access,
  );
  $ekmod_options = array();
/*
  foreach (module_invoke_all('node_operations') as $operation => $array) {
    $options[$operation] = $array['label'];
  }
*/
  foreach (ekmod_operations() as $operation => $array) {
    $ekmod_options[$operation] = $array['label'];
  }
  /*
  $options = array();
  foreach (module_invoke_all('node_operations') as $operation => $array) {
    $options[$operation] = $array['label'];
  }
  
  $form['options']['operation'] = array(
    '#type' => 'select',
    '#title' => t('Operation'),
    '#title_display' => 'invisible',
    '#options' => $options,
    '#default_value' => 'approve',
  );
  */
    $form['options']['operation'] = array(
    '#type' => 'select',
    '#title' => t('Operation'),
    '#title_display' => 'invisible',
    '#options' => $ekmod_options,
    '#default_value' => 'approve',
  );
  /*
  $form['options']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Update'),
    '#validate' => array('node_admin_nodes_validate'),
    '#submit' => array('node_admin_nodes_submit'),
  );
  */
  $form['options']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Update'),
    '#validate' => array('ekmod_admin_nodes_validate'),
    '#submit' => array('ekmod_admin_nodes_submit'),
  );
  // Enable language column if translation module is enabled or if we have any
  // node with language.
  $multilanguage = (module_exists('translation') || db_query_range("SELECT 1 FROM {ekmod_module} WHERE language <> :language", 0, 1, array(':language' => LANGUAGE_NONE))->fetchField());

  // Build the sortable table header.
  $header = array(
    'title' => array('data' => t('Title'), 'field' => 'e.title'),
    'type' => array('data' => t('Type'), 'field' => 'e.type'),
    'author' => t('Author'),
    'status' => array('data' => t('Status'), 'field' => 'e.status'),
    'changed' => array('data' => t('Updated'), 'field' => 'e.changed', 'sort' => 'desc')
  );
  if ($multilanguage) {
    $header['language'] = array('data' => t('Language'), 'field' => 'e.language');
  }
  $header['operations'] = array('data' => t('Operations'));

  $ekmod_query = db_select('ekmod_module','e')->extend('PagerDefault')->extend('TableSort');
//  $query = db_select('node', 'n')->extend('PagerDefault')->extend('TableSort');
//  $query = db_query('SELECT nid FROM {node}, {ekmod_module} WHERE uid = :uid AND status = :status',array(':uid' => $GLOBAL['user']->uid, ':status' => 0))->fetchCol();
//  $query->join('ekmod_module','e');
//  node_build_filter_query($query); //reikia perdaryti

  if (!user_access('bypass ekmod access')) {
    // If the user is able to view their own unpublished nodes, allow them
    // to see these in addition to published nodes. Check that they actually
    // have some unpublished nodes to view before adding the condition.
    if (user_access('view own unpublished content') && $own_unpublished = db_query('SELECT id FROM {ekmod_module} WHERE utr = :utr AND status = :status', array(':utr' => $GLOBALS['user']->uid, ':status' => 0))->fetchCol()) {
      $ekmod_query->condition(db_or()
        ->condition('e.status', 1)
        ->condition('e.id', $own_unpublished, 'IN')
      );
    }
    else {
      // If not, restrict the query to published nodes.
      $query->condition('e.status', 1);
    }
  }
  $id = $ekmod_query
  ->fields('e',array('id'))
  ->limit(50)
  ->orderByHeader($header)
  ->addTag('ekmod_access')
  ->execute()
  ->fetchCol();
//  $eid = ekmod_load($id);
    $eid = entity_load('ekmod',$id);
  /*
  $nids = $query
    ->fields('n',array('id'))
    ->limit(50)
    ->orderByHeader($header)
    ->addTag('ekmod_access')
    ->execute()
    ->fetchCol();
  $nodes = node_load_multiple($nids);
  */
 // $nd = ekmod_load_multiple($id);
  
  // Prepare the list of nodes.
  $languages = language_list();
  $destination = drupal_get_destination();
  $options = array();
  foreach ($eid as $id_) {
    $l_options = $id_->language != LANGUAGE_NONE && isset($languages[$id_->language]) ? array('language' => $languages[$id_->language]) : array();    
    $options[$id_->id] = array(
      'title' => array(
        'data' => array(
          '#type' => 'link',
          '#title' => $id_->title,
          '#href' => 'ekmod/' . $id_->id,
          '#options' => $l_options,
          '#suffix' => ' ' . theme('mark', array('type' => node_mark($id_->id, $id_->changed))),
        ),
      ),
      'type' => /*check_plain(node_type_get_name($id_))*/check_plain(ekmod_type_name_ld($id_)->name),
      'author' => theme('username', array('account' => $id_)),
      'status' => $id_->status ? t('published') : t('not published'),
      'changed' => format_date($id_->changed, 'short'),
    );
    if ($multilanguage) {
      if ($id_->language == LANGUAGE_NONE || isset($languages[$id_->language])) {
        $options[$id_->id]['language'] = $id_->language == LANGUAGE_NONE ? t('Language neutral') : t($languages[$id_->language]->name);
      }
      else {
        $options[$id_->id]['language'] = t('Undefined language (@langcode)', array('@langcode' => $id_->language));
      }
    }
    // Build a list of all the accessible operations for the current node.
    $operations = array();
    if (ekmod_access('update', $id_)) {
      $operations['edit'] = array(
        'title' => t('edit'),
        'href' => 'ekmod/' . $id_->id . '/edit',
        'query' => $destination,
      );
    }
    if (ekmod_access('delete', $id_)) {
      $operations['delete'] = array(
        'title' => t('delete'),
        'href' => 'ekmod/' . $id_->id . '/delete',
        'query' => $destination,
      );
    }
    $options[$id_->id]['operations'] = array();
    if (count($operations) > 1) {
      // Render an unordered list of operations links.
      $options[$id_->id]['operations'] = array(
        'data' => array(
          '#theme' => 'links__node_operations',
          '#links' => $operations,
          '#attributes' => array('class' => array('links', 'inline')),
        ),
      );
    }
    elseif (!empty($operations)) {
      // Render the first and only operation as a link.
      $link = reset($operations);
      $options[$id_->id]['operations'] = array(
        'data' => array(
          '#type' => 'link',
          '#title' => $link['title'],
          '#href' => $link['href'],
          '#options' => array('query' => $link['query']),
        ),
      );
    }    
  }

  // Only use a tableselect when the current user is able to perform any
  // operations.
  if ($ekmod_access) {
    $form['ekmod_module'] = array(
      '#type' => 'tableselect',
      '#header' => $header,
      '#options' => $options,
      '#empty' => t('No content available.'),
    );
  }
  // Otherwise, use a simple table.
  else {
    $form['ekmod_module'] = array(
      '#theme' => 'table',
      '#header' => $header,
      '#rows' => $options,
      '#empty' => t('No content available.'),
    );
  }

  $form['pager'] = array('#markup' => theme('pager'));
  return $form;
}

/**
 * Validate node_admin_nodes form submissions.
 *
 * Check if any nodes have been selected to perform the chosen
 * 'Update option' on.
 */
function ekmod_admin_nodes_validate($form, &$form_state) {
  // Error if there are no items to select.
  if (!is_array($form_state['values']['nodes']) || !count(array_filter($form_state['values']['nodes']))) {
    form_set_error('', t('No items selected.'));
  }
}

/**
 * Process node_admin_nodes form submissions.
 *
 * Execute the chosen 'Update option' on the selected nodes.
 */
function ekmod_admin_nodes_submit($form, &$form_state) {
  $operations = module_invoke_all('node_operations');
  $operation = $operations[$form_state['values']['operation']];
  // Filter out unchecked nodes
  $nodes = array_filter($form_state['values']['nodes']);
  if ($function = $operation['callback']) {
    // Add in callback arguments if present.
    if (isset($operation['callback arguments'])) {
      $args = array_merge(array($nodes), $operation['callback arguments']);
    }
    else {
      $args = array($nodes);
    }
    call_user_func_array($function, $args);

    cache_clear_all();
  }
  else {
    // We need to rebuild the form to go to a second step. For example, to
    // show the confirmation form for the deletion of nodes.
    $form_state['rebuild'] = TRUE;
  }
}

function ekmod_multiple_delete_confirm($form, &$form_state, $nodes) {
  $form['nodes'] = array('#prefix' => '<ul>', '#suffix' => '</ul>', '#tree' => TRUE);
  // array_filter returns only elements with TRUE values
  foreach ($nodes as $nid => $value) {
    $title = db_query('SELECT title FROM {ekmod_module} WHERE id = :id', array(':id' => $nid))->fetchField();
    $form['nodes'][$nid] = array(
      '#type' => 'hidden',
      '#value' => $nid,
      '#prefix' => '<li>',
      '#suffix' => check_plain($title) . "</li>\n",
    );
  }
  $form['operation'] = array('#type' => 'hidden', '#value' => 'delete');
  $form['#submit'][] = 'node_multiple_delete_confirm_submit';
  $confirm_question = format_plural(count($nodes),
                                  'Are you sure you want to delete this item?',
                                  'Are you sure you want to delete these items?');
  return confirm_form($form,
                    $confirm_question,
                    'admin/content', t('This action cannot be undone.'),
                    t('Delete'), t('Cancel'));
}

function ekmod_multiple_delete_confirm_submit($form, &$form_state) {
  if ($form_state['values']['confirm']) {
    node_delete_multiple(array_keys($form_state['values']['nodes']));
    $count = count($form_state['values']['nodes']);
    watchdog('content', 'Deleted @count posts.', array('@count' => $count));
    drupal_set_message(format_plural($count, 'Deleted 1 post.', 'Deleted @count posts.'));
  }
  $form_state['redirect'] = 'admin/content';
}


/**
 * Menu callback: confirm rebuilding of permissions.
 */
function ekmod_configure_rebuild_confirm() {
  return confirm_form(array(), t('Are you sure you want to rebuild the permissions on site content?'),
                  'admin/reports/status', t('This action rebuilds all permissions on site content, and may be a lengthy process. This action cannot be undone.'), t('Rebuild permissions'), t('Cancel'));
}

/**
 * Handler for wipe confirmation
 */
function ekmod_configure_rebuild_confirm_submit($form, &$form_state) {
  ekmod_access_rebuild(TRUE);
  $form_state['redirect'] = 'admin/reports/status';
}
/**
 * Convert numerical value(s) within a delimited string to integer(s).
 *
 * Explode a space delimited string e.g. 'a b 2' and cast numeric string values
 * to integers.
 *
 * @param $string
 *   A space delimited string.
 *
 * @return
 *   An array containing strings and integers.
 */
function _explode_to_int($string) {
  foreach (explode(' ', $string) as $value) {
    $output[] = (is_numeric($value)) ? (int)$value : $value;
  }
  return $output;
}

/**
 * Validate a delimited string of keys and/or keycodes.
 *
 * @param $input
 *   A delimited string of keys and/or keycodes to validate.
 *
 * @return
 *   TRUE if $input is valid otherwise FALSE.
 */
function _validate_keys_string($input) {
  foreach (explode(' ', $input) as $value) {
    $items[] = preg_match('!^(?:[a-z]|\d{1,3})$!i', $value);
  }
  return in_array(FALSE, $items);
}

/**
 * Validate a hex color value.
 *
 * @param $input
 *   The string to validate.
 *
 * @return
 *   TRUE if $input is a valid hex color value (e.g. 000 or 000000)
 */
function _validate_hex_color($input, $long = FALSE) {
  if ($long == TRUE) {
    return preg_match('!^[a-f0-9]{6}$!i', $input);
  }
  else {
    return preg_match('!^[a-f0-9]{3}([a-f0-9]{3})?$!i', $input);
  }
}

/**
 * Filter values that are not strings from an array.
 *
 * @param $array
 *   An array to filter.
 *
 * @return $output
 *   The filtered array.
 */
function _array_to_string_values($array) {
  $output = array();
  foreach ($array as $value) {
    if (is_string($value)) {
      $output[] = $value;
    }
  }
  return $output;
}

function ekmod_manage($form, &$form_state, $type = FALSE) {
	$form['#ekmod_type'] = 1;
	return $form;
}
/**
 * Returns HTML for a node type description for the content type admin overview page.
 *
 * @param $variables
 *   An associative array containing:
 *   - name: The human-readable name of the content type.
 *   - type: An object containing the 'type' (machine name) and 'description' of
 *     the content type.
 *
 * @ingroup themeable
 */
function theme_ekmod($variables) {
  $name = $variables['name'];
  $type = $variables['type'];
  
  $output = check_plain($name);
  $output .= ' <small>' . t('(Machine name: @type)', array('@type' => $type->type)) . '</small>';
  $output .= '<div class="description">' . filter_xss_admin($type->description) . '</div>';
  return $output;
}

function theme_ekmod_types_managed(&$variables) {

}

function ekmod_perm() {
  $ekmod_access = user_access('administer ekmod');
  // Enable language column if translation module is enabled or if we have any
  // node with language.
  $multilanguage = (module_exists('translation') || db_query_range("SELECT 1 FROM {ekmod_module} WHERE language <> :language", 0, 1, array(':language' => LANGUAGE_NONE))->fetchField());

  // Build the sortable table header.
  $header = array(
  	'id' => array('data' => t('Ident nr.'),'field' => 'e.id'),
    'title' => array('data' => t('Title'), 'field' => 'e.title'),
    'type' => array('data' => t('Type'), 'field' => 'e.type'),
    'author' => t('Author'),
    'status' => array('data' => t('Status'), 'field' => 'e.status'),
    'changed' => array('data' => t('Updated'), 'field' => 'e.changed', 'sort' => 'desc')
  );
  if ($multilanguage) {
    $header['language'] = array('data' => t('Language'), 'field' => 'e.language');
  }
  $header['view']   = array('data' => t('View'),   'field' => 'e.grant_view');
  $header['update'] = array('data' => t('Update'), 'field' => 'e.grant_update');
  $header['delete'] = array('data' => t('Delete'), 'field' => 'e.grant_delete');
 // $header['operations'] = array('data' => t('Operations'));

  $ekmod_query = db_select('ekmod_module','e')->extend('PagerDefault')->extend('TableSort');

  if (!user_access('bypass ekmod access')) {
    // If the user is able to view their own unpublished nodes, allow them
    // to see these in addition to published nodes. Check that they actually
    // have some unpublished nodes to view before adding the condition.
    if (user_access('view own unpublished content') && $own_unpublished = db_query('SELECT id FROM {ekmod_module} WHERE utr = :utr AND status = :status', array(':utr' => $GLOBALS['user']->uid, ':status' => 0))->fetchCol()) {
      $ekmod_query->condition(db_or()
        ->condition('e.status', 1)
        ->condition('e.id', $own_unpublished, 'IN')
      );
    }
    else {
      // If not, restrict the query to published nodes.
      $query->condition('e.status', 1);
    }
  }
  $id = $ekmod_query
  ->fields('e',array('id'))
  ->limit(50)
  ->orderByHeader($header)
  ->addTag('ekmod_access')
  ->execute()
  ->fetchCol();
//  $eid = ekmod_load($id);
    $eid = entity_load('ekmod',$id);
  
  // Prepare the list of nodes.
  $languages = language_list();
  $destination = drupal_get_destination();
  $options = array();
  foreach ($eid as $id_) {
    $l_options = $id_->language != LANGUAGE_NONE && isset($languages[$id_->language]) ? array('language' => $languages[$id_->language]) : array();
    debug($eid);
    debug($id_);
    $options[$id_->id] = array(
      'title' => array(
        'data' => array(
          '#type' => 'link',
          '#title' => $id_->title,
          '#href' => 'ekmod/' . $id_->id,
          '#options' => $l_options,
          '#suffix' => ' ' . theme('mark', array('type' => node_mark($id_->id, $id_->changed))),
        ),
      ),
      'type' => check_plain(ekmod_type_name_ld($id_)->name)/*check_plain(node_type_get_name($id_))*/,
      'author' => theme('username', array('account' => $id_)),
      'status' => $id_->status ? t('published') : t('not published'),
      'changed' => format_date($id_->changed, 'short'),
    );
    if ($multilanguage) {
      if ($id_->language == LANGUAGE_NONE || isset($languages[$id_->language])) {
        $options[$id_->id]['language'] = $id_->language == LANGUAGE_NONE ? t('Language neutral') : t($languages[$id_->language]->name);
      }
      else {
        $options[$id_->id]['language'] = t('Undefined language (@langcode)', array('@langcode' => $id_->language));
      }
    }
  
  }

  // Only use a tableselect when the current user is able to perform any
  // operations.
  if ($ekmod_access) {
    $form['ekmod_module'] = array(
      '#type' => 'tableselect',
      '#header' => $header,
      '#options' => $options,
      '#empty' => t('No content available.'),
    );
  }
  // Otherwise, use a simple table.
  else {
    $form['ekmod_module'] = array(
      '#theme' => 'table',
      '#header' => $header,
      '#rows' => $options,
      '#empty' => t('No content available.'),
    );
  }

  $form['pager'] = array('#markup' => theme('pager'));
  return $form;	
}

function field_ui_enabled() {
	  return $existing = db_query("SELECT status FROM {system} WHERE type = :type AND name = :name", array(
        ':type' => 'module',
        ':name' => 'field_ui',
      ))->fetchObject();
}

function enable_field_ui_module() {
  $module = 'field_ui';	 
  $existing = db_query("SELECT status FROM {system} WHERE type = :type AND name = :name", array(
      ':type' => 'module',
      ':name' => $module,
    ))->fetchObject();
    if ($existing->status == 0) {
      module_enable('field_ui');      
      // Required for module installation checks.
      include_once DRUPAL_ROOT . '/includes/install.inc';
    
      $modules_installed = array();
      $modules_enabled = array();
      // Load the module's code.
      drupal_load('module', $module);
      module_load_install($module);

      // Update the database and module list to reflect the new module. This
      // needs to be done first so that the module's hook implementations,
      // hook_schema() in particular, can be called while it is being
      // installed.
      db_update('system')->fields(array('status' => 1))->condition('type', 'module')->condition('name', $module)->execute();
      // Refresh the module list to include it.
      // Refresh the module list to include it.
      system_list_reset();
      module_list(TRUE);
      module_implements('', FALSE, TRUE);
      _system_update_bootstrap_status();
      // Update the registry to include it.
      registry_update();
      // Refresh the schema to include it.
      drupal_get_schema(NULL, TRUE);
      // Update the theme registry to include it.
      drupal_theme_rebuild();
      // Clear entity cache.
      entity_info_cache_clear();

      // Now install the module if necessary.
      if (drupal_get_installed_schema_version($module, TRUE) == SCHEMA_UNINSTALLED) {
        drupal_install_schema($module);

        // Set the schema version to the number of the last update provided
        // by the module.
        $versions = drupal_get_schema_versions($module);
        $version = $versions ? max($versions) : SCHEMA_INSTALLED;

        // If the module has no current updates, but has some that were
        // previously removed, set the version to the value of
        // hook_update_last_removed().
        if ($last_removed = module_invoke($module, 'update_last_removed')) {
          $version = max($version, $last_removed);
        }
        drupal_set_installed_schema_version($module, $version);
        // Allow the module to perform install tasks.
        module_invoke($module, 'install');
        // Record the fact that it was installed.
        $modules_installed[] = $module;
        watchdog('system', '%module module installed.', array('%module' => $module), WATCHDOG_INFO);
      }

      // Enable the module.
      module_invoke($module, 'enable');
      // Record the fact that it was enabled.
      $modules_enabled[] = $module;
      // If any modules were newly installed, invoke hook_modules_installed().
      if (!empty($modules_installed)) {
        module_invoke_all('modules_installed', $modules_installed);
      }
    
      // If any modules were newly enabled, invoke hook_modules_enabled().
      if (!empty($modules_enabled)) {
        module_invoke_all('modules_enabled', $modules_enabled);
      }
    }
}