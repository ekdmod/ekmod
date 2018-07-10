<?php 
/**
 * Form constructor for the field display settings for a given view mode.
 *
 * @see field_ui_display_overview_multistep_submit()
 * @see field_ui_display_overview_form_submit()
 * @ingroup forms
 */
function ui_field_display_ekmod_form($form, &$form_state, $bundle) {
  $ekmod = ekmod_entity_info();
  $admin_path = $ekmod['ekmod']['bundles'][$bundle]['admin']['real path'];

  // Gather type information.
  //$instances = field_info_instances($entity_type, $bundle);
$query = db_select('field_config_instance', 'fci', array('fetch' => PDO::FETCH_ASSOC));
  $query->join('field_config', 'fc', 'fc.id = fci.field_id');
  $query->fields('fci');
  $query->condition('fci.bundle', array_keys($ekmod['ekmod']['bundles']));
  $include_additional = array();
  $include_inactive = isset($include_additional['include_inactive']) && $include_additional['include_inactive'];
  $include_deleted = isset($include_additional['include_deleted']) && $include_additional['include_deleted'];
if (!$include_inactive) {
    $query
      ->condition('fc.active', 1)
      ->condition('fc.storage_active', 1);
  }
  if (!$include_deleted) {
    $query->condition('fc.deleted', 0);
    $query->condition('fci.deleted', 0);
  }
  $instances = array();
  $results = $query->execute();
foreach ($results as $record) {
    // Filter out instances on unknown entity types (for instance because the
    // module exposing them was disabled).
//    $entity_info = entity_get_info($record['entity_type']);
//    if ($include_inactive || $entity_info) {
	  if ($include_inactive || $ekmod) {
      $instance = unserialize($record['data']);
      $instance['id'] = $record['id'];
      $instance['field_id'] = $record['field_id'];
      $instance['field_name'] = $record['field_name'];
      $instance['entity_type'] = $record['entity_type'];
//    $instance['bundle'] = $record['bundle'];
      $instance['deleted'] = $record['deleted'];
//    $instance[$record['bundle']] = $instance['field_name'];
 //   module_invoke_all('field_read_instance', $instance);
      $instances[$record['bundle']] = $instance;
    }
  }
  $field_types = field_info_field_types();
 //$extra_fields = field_info_extra_fields('ekmod', $bundle, 'display');

  $form_state += array(
    'formatter_settings_edit' => NULL,
  );

  $form += array(
    '#entity_type' => 'ekmod',
    '#bundle' => $bundle,
    '#view_mode' => $view_mode,
    '#fields' => array_keys($instances),
    '#extra' => array_keys($extra_fields),
  );

  if (empty($instances) && empty($extra_fields)) {
    drupal_set_message(t('There are no fields yet added. You can add new fields on the <a href="@link">Manage fields</a> page.', array('@link' => url($admin_path . '/fields'))), 'warning');
    return $form;
  }

  $table = array(
    '#type' => 'field_ui_table',
    '#tree' => TRUE,
    '#header' => array(
      t('Field'),
      t('Weight'),
      t('Parent'),
      t('Label'),
      array('data' => t('Format'), 'colspan' => 3),
    ),
    '#regions' => array(
      'visible' => array('message' => t('No field is displayed.')),
      'hidden' => array('title' => t('Hidden'), 'message' => t('No field is hidden.')),
    ),
    '#parent_options' => array(),
    '#attributes' => array(
      'class' => array('field-ui-overview'),
      'id' => 'field-display-overview',
    ),
    // Add Ajax wrapper.
    '#prefix' => '<div id="field-display-overview-wrapper">',
    '#suffix' => '</div>',
  );

  $field_label_options = array(
    'above' => t('Above'),
    'inline' => t('Inline'),
    'hidden' => t('<Hidden>'),
  );
  $extra_visibility_options = array(
    'visible' => t('Visible'),
    'hidden' => t('Hidden'),
  );

  // Field rows.
  foreach ($instances as $name => $instance) {
    $field = field_info_field($instance['field_name']);
    $display = $instance['display'][$view_mode];
    $table[$name] = array(
      '#attributes' => array('class' => array('draggable', 'tabledrag-leaf')),
      '#row_type' => 'field',
      '#region_callback' => 'field_ui_display_overview_row_region',
      '#js_settings' => array(
        'rowHandler' => 'field',
        'defaultFormatter' => $field_types[$field['type']]['default_formatter'],
      ),
      'human_name' => array(
        '#markup' => check_plain($instance['label']),
      ),
      'weight' => array(
        '#type' => 'textfield',
        '#title' => t('Weight for @title', array('@title' => $instance['label'])),
        '#title_display' => 'invisible',
        '#default_value' => $display['weight'],
        '#size' => 3,
        '#attributes' => array('class' => array('field-weight')),
      ),
      'parent_wrapper' => array(
        'parent' => array(
          '#type' => 'select',
          '#title' => t('Label display for @title', array('@title' => $instance['label'])),
          '#title_display' => 'invisible',
          '#options' => $table['#parent_options'],
          '#empty_value' => '',
          '#attributes' => array('class' => array('field-parent')),
          '#parents' => array('fields', $name, 'parent'),
        ),
        'hidden_name' => array(
          '#type' => 'hidden',
          '#default_value' => $name,
          '#attributes' => array('class' => array('field-name')),
        ),
      ),
      'label' => array(
        '#type' => 'select',
        '#title' => t('Label display for @title', array('@title' => $instance['label'])),
        '#title_display' => 'invisible',
        '#options' => $field_label_options,
        '#default_value' => $display['label'],
      ),
    );

    $formatter_options = field_ui_formatter_options($field['type']);
    $formatter_options['hidden'] = t('<Hidden>');
    $table[$name]['format'] = array(
      'type' => array(
        '#type' => 'select',
        '#title' => t('Formatter for @title', array('@title' => $instance['label'])),
        '#title_display' => 'invisible',
        '#options' => $formatter_options,
        '#default_value' => $display['type'],
        '#parents' => array('fields', $name, 'type'),
        '#attributes' => array('class' => array('field-formatter-type')),
      ),
      'settings_edit_form' => array(),
    );

    // Formatter settings.

    // Check the currently selected formatter, and merge persisted values for
    // formatter settings.
    if (isset($form_state['values']['fields'][$name]['type'])) {
      $formatter_type = $form_state['values']['fields'][$name]['type'];
    }
    else {
      $formatter_type = $display['type'];
    }
    if (isset($form_state['formatter_settings'][$name])) {
      $settings = $form_state['formatter_settings'][$name];
    }
    else {
      $settings = $display['settings'];
    }
    $settings += field_info_formatter_settings($formatter_type);

    $instance['display'][$view_mode]['type'] = $formatter_type;
    $formatter = field_info_formatter_types($formatter_type);
    $instance['display'][$view_mode]['module'] = $formatter['module'];
    $instance['display'][$view_mode]['settings'] = $settings;

    // Base button element for the various formatter settings actions.
    $base_button = array(
      '#submit' => array('field_ui_display_overview_multistep_submit'),
      '#ajax' => array(
        'callback' => 'field_ui_display_overview_multistep_js',
        'wrapper' => 'field-display-overview-wrapper',
        'effect' => 'fade',
      ),
      '#field_name' => $name,
    );

    if ($form_state['formatter_settings_edit'] == $name) {
      // We are currently editing this field's formatter settings. Display the
      // settings form and submit buttons.
      $table[$name]['format']['settings_edit_form'] = array();

      $settings_form = array();
      $function = $formatter['module'] . '_field_formatter_settings_form';
      if (function_exists($function)) {
        $settings_form = $function($field, $instance, $view_mode, $form, $form_state);
      }

      if ($settings_form) {
        $table[$name]['format']['#cell_attributes'] = array('colspan' => 3);
        $table[$name]['format']['settings_edit_form'] = array(
          '#type' => 'container',
          '#attributes' => array('class' => array('field-formatter-settings-edit-form')),
          '#parents' => array('fields', $name, 'settings_edit_form'),
          'label' => array(
            '#markup' => t('Format settings:') . ' <span class="formatter-name">' . $formatter['label'] . '</span>',
          ),
          'settings' => $settings_form,
          'actions' => array(
            '#type' => 'actions',
            'save_settings' => $base_button + array(
              '#type' => 'submit',
              '#name' => $name . '_formatter_settings_update',
              '#value' => t('Update'),
              '#op' => 'update',
            ),
            'cancel_settings' => $base_button + array(
              '#type' => 'submit',
              '#name' => $name . '_formatter_settings_cancel',
              '#value' => t('Cancel'),
              '#op' => 'cancel',
              // Do not check errors for the 'Cancel' button, but make sure we
              // get the value of the 'formatter type' select.
              '#limit_validation_errors' => array(array('fields', $name, 'type')),
            ),
          ),
        );
        $table[$name]['#attributes']['class'][] = 'field-formatter-settings-editing';
      }
    }
    else {
      // Display a summary of the current formatter settings.
      $summary = module_invoke($formatter['module'], 'field_formatter_settings_summary', $field, $instance, $view_mode);
      $table[$name]['settings_summary'] = array();
      $table[$name]['settings_edit'] = array();
      if ($summary) {
        $table[$name]['settings_summary'] = array(
          '#markup' => '<div class="field-formatter-summary">' . $summary . '</div>',
          '#cell_attributes' => array('class' => array('field-formatter-summary-cell')),
        );
        $table[$name]['settings_edit'] = $base_button + array(
          '#type' => 'image_button',
          '#name' => $name . '_formatter_settings_edit',
          '#src' => 'misc/configure.png',
          '#attributes' => array('class' => array('field-formatter-settings-edit'), 'alt' => t('Edit')),
          '#op' => 'edit',
          // Do not check errors for the 'Edit' button, but make sure we get
          // the value of the 'formatter type' select.
          '#limit_validation_errors' => array(array('fields', $name, 'type')),
          '#prefix' => '<div class="field-formatter-settings-edit-wrapper">',
          '#suffix' => '</div>',
        );
      }
    }
  }

  // Non-field elements.
  foreach ($extra_fields as $name => $extra_field) {
    $display = $extra_field['display'][$view_mode];
    $table[$name] = array(
      '#attributes' => array('class' => array('draggable', 'tabledrag-leaf')),
      '#row_type' => 'extra_field',
      '#region_callback' => 'field_ui_display_overview_row_region',
      '#js_settings' => array('rowHandler' => 'field'),
      'human_name' => array(
        '#markup' => check_plain($extra_field['label']),
      ),
      'weight' => array(
        '#type' => 'textfield',
        '#title' => t('Weight for @title', array('@title' => $extra_field['label'])),
        '#title_display' => 'invisible',
        '#default_value' => $display['weight'],
        '#size' => 3,
        '#attributes' => array('class' => array('field-weight')),
      ),
      'parent_wrapper' => array(
        'parent' => array(
          '#type' => 'select',
          '#title' => t('Parents for @title', array('@title' => $extra_field['label'])),
          '#title_display' => 'invisible',
          '#options' => $table['#parent_options'],
          '#empty_value' => '',
          '#attributes' => array('class' => array('field-parent')),
          '#parents' => array('fields', $name, 'parent'),
        ),
        'hidden_name' => array(
          '#type' => 'hidden',
          '#default_value' => $name,
          '#attributes' => array('class' => array('field-name')),
        ),
      ),
      'empty_cell' => array(
        '#markup' => '&nbsp;',
      ),
      'format' => array(
        'type' => array(
          '#type' => 'select',
          '#title' => t('Visibility for @title', array('@title' => $extra_field['label'])),
          '#title_display' => 'invisible',
          '#options' => $extra_visibility_options,
          '#default_value' => $display['visible'] ? 'visible' : 'hidden',
          '#parents' => array('fields', $name, 'type'),
          '#attributes' => array('class' => array('field-formatter-type')),
        ),
      ),
      'settings_summary' => array(),
      'settings_edit' => array(),
    );
  }

  $form['fields'] = $table;

  // Custom display settings.
  if ($view_mode == 'default') {
    $form['modes'] = array(
      '#type' => 'fieldset',
      '#title' => t('Custom display settings'),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
    );
    // Collect options and default values for the 'Custom display settings'
    // checkboxes.
    $options = array();
    $default = array();
 //   $entity_info = entity_get_info($entity_type);
 //   $view_modes = $ekmod['view modes'];
    $view_mode_settings = field_view_mode_settings('ekmod', $bundle);
    foreach ($ekmod['view modes'] as $view_mode_name => $view_mode_info) {
      $options[$view_mode_name] = $view_mode_info['label'];
      if (!empty($view_mode_settings[$view_mode_name]['custom_settings'])) {
        $default[] = $view_mode_name;
      }
    }
    $form['modes']['view_modes_custom'] = array(
      '#type' => 'checkboxes',
      '#title' => t('Use custom display settings for the following view modes'),
      '#options' => $options,
      '#default_value' => $default,
    );
  }

  // In overviews involving nested rows from contributed modules (i.e
  // field_group), the 'format type' selects can trigger a series of changes in
  // child rows. The #ajax behavior is therefore not attached directly to the
  // selects, but triggered by the client-side script through a hidden #ajax
  // 'Refresh' button. A hidden 'refresh_rows' input tracks the name of
  // affected rows.
  $form['refresh_rows'] = array('#type' => 'hidden');
  $form['refresh'] = array(
    '#type' => 'submit',
    '#value' => t('Refresh'),
    '#op' => 'refresh_table',
    '#submit' => array('field_ui_display_overview_multistep_submit'),
    '#ajax' => array(
      'callback' => 'field_ui_display_overview_multistep_js',
      'wrapper' => 'field-display-overview-wrapper',
      'effect' => 'fade',
      // The button stays hidden, so we hide the Ajax spinner too. Ad-hoc
      // spinners will be added manually by the client-side script.
      'progress' => 'none',
    ),
  );

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array('#type' => 'submit', '#value' => t('Save'));

  $form['#attached']['js'][] = drupal_get_path('module', 'field_ui') . '/field_ui.js';
  $form['#attached']['css'][] = drupal_get_path('module', 'field_ui') . '/field_ui.css';

  // Add tabledrag behavior.
  $form['#attached']['drupal_add_tabledrag'][] = array('field-display-overview', 'order', 'sibling', 'field-weight');
  $form['#attached']['drupal_add_tabledrag'][] = array('field-display-overview', 'match', 'parent', 'field-parent', 'field-parent', 'field-name');

  return $form;
}
?>