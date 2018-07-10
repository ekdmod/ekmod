<?php

/**
 * @file
 * Page callbacks for adding, editing, deleting, and revisions management for content.
 */
function gallery_render() {
	ekmod_dt();
  	return ekmod_page(1);
}

function ekmod_add_page() {
	$item = menu_get_item('ekmod/%');
	$links = system_admin_menu_block($item);
	// Bypass the node/add listing if only one content type is available.
	if (count($links) == 1) {
    	$item = array_shift($links);
    	drupal_goto($item['href']);
	}
	
	return theme('node_add_list', array('items' => $links));
}
/**
 * Returns a node submission form.
 */
function ekmod_add($type) {
  global $user;

  $types = ekmod_load_types()->types;
  $type = isset($type) ? str_replace('-', '_', $type) : NULL;
  if (empty($types[$type])) {
 	return MENU_NOT_FOUND;
  }
  $ekmod = entity_get_controller('ekmod')->create($type);
  // $ekmod = ekmod_entity_controller()->create($type);
  // $output = drupal_get_form($type . '_ekmod_form', $ekmod);
  $cr = (object) array('utr' => $user->uid, 'name' => (isset($user->name) ? $user->name : ''), 'type' => $type, 'language' => LANGUAGE_NONE);  
  drupal_set_title(t('Create @name', array('@name' => $types[$type]->name)), PASS_THROUGH);  
  $output = drupal_get_form($type . '_form', $ekmod);

  return $output;
}


/**
 * Menu callback; presents the node editing form.
 */
function ekmod_page_edit($ekmod) {
  $types = ekmod_load_types()->types;
  drupal_set_title(t('<em>Edit @type</em> @title', array('@type' => $types[$ekmod->type]->name, '@title' => $ekmod->title)), PASS_THROUGH);
  return drupal_get_form($ekmod->type . '_form', $ekmod);
}


/**
 * Returns HTML for a list of available node types for node creation.
 *
 * @param $variables
 *   An associative array containing:
 *   - content: An array of content types.
 *
 * @ingroup themeable
 */
function theme_ekmod_add_list($variables) {
   debug($variables);
  $content = $variables['content'];
  $output = '';

  if ($content) {
    $output = '<dl class="ekmod-type-list">';
    foreach ($content as $item) {
      $output .= '<dt>' . l($item['title'], $item['href'], $item['localized_options']) . '</dt>';
      $output .= '<dd>' . filter_xss_admin($item['description']) . '</dd>';
    }
    $output .= '</dl>';
  }
  else {
    $output = '<p>' . t('You have not created any content types yet. Go to the <a href="@create-content">content type creation page</a> to add a new content type.', array('@create-content' => url('admin/structure/types/add'))) . '</p>';
  }
  return $output;
}

function ekmod_form($form, &$form_state, $ekmod) {
  $ekmod->language = LANGUAGE_NONE;
  global $user;
  $form['#attached']['js'] = array(drupal_get_path('module', 'ekmod') . '/ekmod_form.js',);
  $type = $ekmod->type;
  // During initial form build, add the node entity to the form state for use
  // during form building and processing. During a rebuild, use what is in the
  // form state.
  if (!isset($form_state['ekmod'])) {
    if (!isset($ekmod->title)) {
      $ekmod->title = NULL;
    }
	// node_object_prepare($ekmod);
    $form_state['ekmod'] = $ekmod;
  } else {
    $ekmod = $form_state['ekmod'];
  }

  // Some special stuff when previewing a node.
  if (isset($form_state['ekmod_preview'])) {
    $form['#prefix'] = $form_state['ekmod_preview'];
    $ekmod->in_preview = TRUE;
  }else {
    unset($ekmod->in_preview);
  }
  
  // Basic node information.
  // These elements are just values so they are not even sent to the client.
  foreach (array('id', 'ver', 'utr', 'created', 'type', 'language') as $key) {
    $form[$key] = array(
      '#type' => 'value',
      '#value' => isset($ekmod->$key) ? $ekmod->$key : NULL,
    );
  }
  
  $form['#id'] = 'ekmod-form';
  $form['#ekmod'] = $ekmod;
  $form_state['ekmod'] = $ekmod;
  $types = ekmod_load_types()->types;
  $has_title = isset($types[$type]) && isset($types[$type]->has_title) ? TRUE : FALSE;
    $cat = array();
    $op_1 = db_query('SELECT DISTINCT type, ct, title FROM ekmod_module');
    $op_2 = db_query('SELECT DISTINCT type, ct, category FROM ekmod_module');
    $str_sub_tp = substr($type,strripos($type,'_')+1);
 // $ct_type = substr($type,strripos($type,'category')+1);
    $tp = db_query("SELECT DISTINCT type FROM ekmod_module WHERE type LIKE '%".$str_sub_tp."'");
 // debug($tp);
  while($l = $tp->fetch()) {
    $cat[$l->type]['Title'][0]['new'] = t('- new -');
    $cat[$l->type]['Title'][1]['new'] = t('- new -');
    $cat[$l->type]['Category'][0]['new'] = t('- new -');
    $cat[$l->type]['Category'][1]['new'] = t('- new -');
  }
  while($obj_1 = $op_1->fetch())
  {
    //$cat_ct[] = $obj_3->title;
    $cat[$obj_1->type]['Title'][$obj_1->ct][] = $obj_1->title;
  }
  while($obj_2 = $op_2->fetch())
  {
    // $cat_ct[] = $obj_3->title;
    $cat[$obj_2->type]['Category'][$obj_2->ct][] = $obj_2->category;
  }
  $cat['ct'] = array(0 => array( 0 => 'ekmod_album_category_image', 1 => 0, 2 => 'Title') , 1 => array(0 => 'ekmod_album_category_image', 1 => 1, 2 => 'Title') , 2 => array(0 => 'ekmod_album_image', 1 => 0 , 2 => 'Title'), 3 => array(0 => 'ekmod_album_image', 1 => 1, 2 => 'Title'),);
  //drupal_add_js(array('ekmod_form' => array('category' => array(1 => $op_1, 2 => $op_2 ),'Album' => array(1 => $op_3, 2 => $op_5 )),), 'setting');
  drupal_add_js(array('ekmod_form' => $cat), 'setting');
  $form['category_hidden'] = array( 
    '#type' => 'hidden',
    '#default_value' => '',
  );
  // $ekmod->settings = $cat;
  //visual imaging //visual media //system //imaging resource
  if($has_title) {
	$form['title'] = array (
		'#type' => 'textfield',
		'#title' => check_plain($types[$type]->title_label), // $type->title_label
		'#required' => TRUE,
		'#default_value' => $ekmod->title,
		'#maxlength' => 255,
	//	'#weight' => -5,
	);

	$form['type'] = array(
		'#type' => 'select',
//'#default_value' => 1,//
		'#options' => array(0 => t('Category'), 1 => t('Album item view')),
		'#title' => t('Type'),

	);

      if(!empty($cat))
      {
        $form['category'] = array(
        	'#type' => 'select',
       		//'#empty_value' => ' - none - ',
        	'#title' => t('Add content :'),
      /*
        	'#states' => array(
          'invisible' => array( 
          ':input[name="add"]' => array('value' => '0'),),
        	),
      */
       	'#default_value' => array('new' => '- new -',),
       	'#options' => $cat['ekmod_album_category_image']['Title'][0],
      	//'#process' => array('ekmod_select_js','form_process_select','ajax_process_form'),
      	'#description' => t(''),
      	'#weight' => -5,
        );
      }
	}

  $form['additional_settings'] = array(
    '#type' => 'vertical_tabs',
    '#weight' => 99,
  );
  
    // @todo D8: Remove. Modules should access the node using $form_state['node'].
  $form['#ekmod'] = $ekmod;
  $ekmod->revision = false;
  // Add a log field if the "Create new revision" option is checked, or if the
  // current user has the ability to check that option.
  $form['revision_information'] = array(
    '#type' => 'fieldset',
    '#title' => t('Revision information'),
    '#collapsible' => TRUE,
    // Collapsed by default when "Create new revision" is unchecked
    '#collapsed' => !$ekmod->revision,
    '#group' => 'additional_settings',
    '#attributes' => array(
      'class' => array('ekmod-form-revision-information'),
    ),
    '#attached' => array(
      'js' => array(drupal_get_path('module', 'ekmod') . '/node_ekmod.js'),
    ),
    '#weight' => 20,
    '#access' => $ekmod->revision || user_access('administer ekmod'),
  );
  $form['revision_information']['revision'] = array(
    '#type' => 'checkbox',
    '#title' => t('Create new revision'),
    '#default_value' => $ekmod->revision,
    '#access' => user_access('administer ekmod'),
  );
  // Check the revision log checkbox when the log textarea is filled in.
  // This must not happen if "Create new revision" is enabled by default, since
  // the state would auto-disable the checkbox otherwise.
  if (!$ekmod->revision) {
    $form['revision_information']['revision']['#states'] = array(
      'checked' => array(
        'textarea[name="log"]' => array('empty' => FALSE),
      ),
    );
  }
  $form['revision_information']['log'] = array(
    '#type' => 'textarea',
    '#title' => t('Revision log message'),
    '#rows' => 4,
    '#default_value' => !empty($ekmod->log) ? $ekmod->log : '',
    '#description' => t('Provide an explanation of the changes you are making. This will help other authors understand your motivations.'),
  );
  
	// Add the buttons.
	$form['buttons'] = array();
	$form['buttons']['#weight'] = 100;
	$form['buttons']['submit'] = array(
		'#type' => 'submit',
		'#value' => t('Save'),
		'#weight' => 5,
		'#submit' => array('ekmod_form_submit'),
	);
	if (!empty($ekmod->id)) {
	$form['buttons']['delete'] = array(
		'#access' => user_access('delete ekmod'),
		'#type' => 'submit',
		'#value' => t('Delete'),
		'#weight' => 15,
		'#submit' => array('ekmod_form_delete_submit'),
	);
	}
  // This form uses a button-level #submit handler for the form's main submit
  // action. node_form_submit() manually invokes all form-level #submit handlers
  // of the form. Without explicitly setting #submit, Form API would auto-detect
  // node_form_submit() as submit handler, but that is the button-level #submit
  // handler for the 'Save' action. To maintain backwards compatibility, a
  // #submit handler is auto-suggested for custom node type modules.
  $form['#validate'][] = 'ekmod_form_validate';
  
  field_attach_form('ekmod', $ekmod, $form, $form_state, $ekmod->language);
  
  return $form;
}

	function ekmod_select_js($form, $form_state) {
 
	$ct_value = !empty($form_state['values']['type']) ? $form_state['values']['type'] : 1;	

	$form['type'] = array(
		'#type' => 'select',
		'#default_value' => $ct_value,		
	);


  $val = $ct_value == 1 ? db_query('SELECT DISTINCT title FROM ekmod_module WHERE ekmod_module.type = \'ekmod_album_category_image\' AND ct = 0')->fetchCol() : $op = db_query('SELECT DISTINCT category FROM ekmod_module WHERE ekmod_module.type = \'ekmod_album_image\' AND ct = 0')->fetchCol();
  
   return $form['category'] = array(
    '#type' => 'select',
  
 	'#default_value' => '1',
 	'#options' => array(0 => $ct_value,),

  );
  
//  return $form;
	}


/**
 * Menu callback -- ask for confirmation of node deletion
 */
function ekmod_delete_confirm($form, &$form_state, $ekmod) {
  $form['#ekmod'] = $ekmod;
  // Always provide entity id in the same form key as in the entity edit form.
  $form['id'] = array('#type' => 'value', '#value' => $ekmod->id);
  return confirm_form($form,
    t('Are you sure you want to delete %title?', array('%title' => $ekmod->title)),
    'ekmod/' . $ekmod->id,
    t('This action cannot be undone.'),
    t('Delete'),
    t('Cancel')
  );
}

function ekmod_album_image_trigger_as_form_submit($form, $form_state) {
   return $form['category'] = array(
    '#type' => 'select',
 	'#options' => array(0 => 3),

  );
}
/**
 * Button submit function: handle the 'Delete' button on the node form.
 */
function ekmod_form_delete_submit($form, &$form_state) {
  $destination = array();
  if (isset($_GET['destination'])) {
    $destination = drupal_get_destination();
    unset($_GET['destination']);
  }
  $node = $form['#ekmod'];
  $form_state['redirect'] = array('ekmod/' . $ekmod->id . '/delete', array('query' => $destination));
}

/**
 * Execute node deletion
 */
function ekmod_delete_confirm_submit($form, &$form_state) {
  if ($form_state['values']['confirm']) {
    $ekmod = ekmod_load($form_state['values']['id']);
    $types = ekmod_load_types()->types;
    ekmod_delete($form_state['values']['id']);
    watchdog('content', '@type: deleted %title.', array('@type' => $ekmod->type, '%title' => $ekmod->title));
    drupal_set_message(t('@type %title has been deleted.', array('@type' => $types[$ekmod->type]->name, '%title' => $ekmod->title)));
  }

  $form_state['redirect'] = '<front>';
}

function ekmod_form_validate($form, &$form_state) {

	$ekmod = $form_state['ekmod'];
	// Field validation.
	field_attach_form_validate('ekmod', $ekmod, $form, $form_state);

  	// $form_state['node'] contains the actual entity being edited, but we must
  	// not update it with form values that have not yet been validated, so we
  	// create a pseudo-entity to use during validation.
	//  $node = (object) $form_state['values'];
	//  node_validate($node, $form, $form_state);
	//  entity_form_field_validate('ekmod', $form, $form_state);
}

function ekmod_form_submit($form, &$form_state) {
	global $user;
  
  $ekmod = &$form_state['ekmod'];
    
  // Set the artwork's utr if it's being created at this time.
  if (empty($ekmod->utr)) {
  	$ekmod->utr = $user->uid;
  }
  $ekmod->title = $form_state['values']['title']; 
  $ekmod->category = $form_state['values']['category'];
  $ekmod->ct = $form_state['values']['type'];
  $ekmod->revision = $form_state['values']['revision'];
  // Notify field widgets.
  field_attach_submit('ekmod', $ekmod, $form, $form_state);
  ekmod_save($ekmod);
  // Notify the user.
  drupal_set_message(t('Content added.'));
  
}

?>