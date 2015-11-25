<?php

/**
 * @file
 * Defines the inline entity form controller for Commerce Line Items.
 */

class FieldCollectionInlineEntityFormController extends EntityInlineEntityFormController {

  /**
   * Overrides EntityInlineEntityFormController::tableFields().
   */
  public function tableFields($bundles) {
    $fields = array();
    $fields['field_file_type'] = array(
      'type' => 'field',
      'label' => t('Title'),
      'weight' => 1,
    );

    return $fields;
  }

  /**
   * Overrides EntityInlineEntityFormController::defaultSettings().
   */
  public function defaultSettings() {
    $defaults = parent::defaultSettings();

    // Line items should always be deleted when the order is deleted, they
    // are never managed alone.
    $defaults['delete_references'] = TRUE;

    return $defaults;
  }

  /**
   * Overrides EntityInlineEntityFormController::settingsForm().
   */
  public function settingsForm($field, $instance) {
    $form = parent::settingsForm($field, $instance);

    // Adding existing entities is not supported for field collections.
    $form['allow_existing']['#access'] = FALSE;
    $form['match_operator']['#access'] = FALSE;

    return $form;
  }

  /**
   * Overrides EntityInlineEntityFormController::entityForm().
   */
  public function entityForm($entity_form, &$form_state) {
    $fc_item = $entity_form['#entity'];

    field_attach_form('field_collection_item', $fc_item, $entity_form, $form_state);

    $parents = array();
    foreach ($entity_form['#parents'] as $parent_key) {
      $parents[] = $parent_key;
      if (is_numeric($parent_key)) {
        break;
      }
    }
    $parents[] = '#entity';
    $host_entity = drupal_array_get_nested_value($form_state['complete form'], $parents);
    $entity_form['#hostEntity'] = $host_entity;

    return $entity_form;
  }

  /**
   * Overrides EntityInlineEntityFormController::entityFormValidate().
   *
   * @todo Remove once Commerce gets a quantity #element_validate function.
   */
  public function entityFormValidate($entity_form, &$form_state) {
    $line_item = $entity_form['#entity'];

    field_attach_form_validate('field_collection_item', $line_item, $entity_form, $form_state);
  }

  /**
   * Overrides EntityInlineEntityFormController::entityFormSubmit().
   */
  public function entityFormSubmit(&$entity_form, &$form_state) {
    /**
     * @var FieldCollectionItemEntity $fc_item
     */
    $fc_item = $entity_form['#entity'];

    field_attach_submit('field_collection_item', $fc_item, $entity_form, $form_state);

    if (!empty($fc_item->is_new)) {
      $host_entity = $entity_form['#hostEntity'];
      $class_name = get_class($host_entity);
      $entity_type = ($class_name == 'FieldCollectionItemEntity' ? 'field_collection_item' : 'node');
      $fc_item->setHostEntity($entity_type, $host_entity);
    }
  }

  /**
   * Overrides EntityInlineEntityFormController::removeForm().
   */
  public function removeForm($remove_form, &$form_state) {
    // EntityInlineEntityFormController::removeForm uses the entity label
    // in the confirmation message, but line items don't have any.
    $remove_form['message'] = array(
      '#markup' => '<div>' . t('Are you sure you want to remove this lesson?') . '</div>',
    );

    return $remove_form;
  }

  /**
   * Permanently saves the given entity.
   *
   * @param $entity
   *   The entity to save.
   * @param array $context
   *   Available keys:
   *   - parent_entity_type: The type of the parent entity.
   *   - parent_entity: The parent entity.
   */
  public function save($entity, $context) {
    if (method_exists($entity, 'save')) {
      return $entity->save(TRUE);
    }
  }

}
