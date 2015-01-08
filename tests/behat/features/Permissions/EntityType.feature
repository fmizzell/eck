@api @permissions
Feature: Entity Type Permissions
  As a site administrator
  I want to control who can do what with entity types, bundles and entities
  so users don't get themselves in trouble.

  @setup
  Scenario Outline: This is a set up step
    Given I am logged in as a user with the "Use the administration pages and help,Administer Entity Types,Administer Bundles,Administer Entities,Administer permissions" permissions
    And I visit "/admin/structure/entity-type"
    And I click "Add entity type"
    And I fill in "edit-entity-type-label" with <type_label>
    And I fill in "edit-entity-type-name" with <type>
    And I fill in "edit-bundle-label" with <bundle_label>
    And I fill in "edit-bundle-name" with <bundle>
    And I check "Title"
    And I press the "Save" button
    And I visit <link>
    And I click "Manage fields"
    And I fill in "edit-fields-eck-add-extra-field-field-name" with "title"
    And I fill in "edit-fields-eck-add-extra-field-label" with "Title"
    And I fill in "edit-fields-eck-add-extra-field-widget-type" with "text"
    And I press the "Save" button
    And I visit <link>
    And I click <add_link>
    And I fill in "Title" with <entity_title>
    And I press the "Save" button

    Examples:
      | type_label | type      | bundle_label | bundle | entity_title   | link                                       | add_link  |
      | "Vehicle"  | "vehicle" | "Car"        | "car"  | "Toyota Prius" | "/admin/structure/entity-type/vehicle/car" | "Add Car" |
      | "Animal"   | "animal"  | "Dog"        | "dog"  | "Snoopy"       | "/admin/structure/entity-type/animal/dog"  | "Add Dog" |

  @entity-type @drupal-perm
  Scenario Outline: Only allowed users can access the entity type's overview page
    Given I am logged in as a user with the "Use the administration pages and help" permissions
    And I visit "/admin/structure"
    Then I should not see the text "Entity types"
    Given I am logged in as a user with the <permissions> permissions
    And I visit "/admin/structure"
    And I click "Entity types"
    Then I should get a "200" HTTP response

    Examples:
      | permissions                                                     |
      | "Use the administration pages and help,List Entity Types"       |
      | "Use the administration pages and help,Administer Entity Types" |

  @entity-type @drupal-perm
  Scenario Outline: Only allowed users can add entity types from the overview page
    Given I am logged in as a user with the "Use the administration pages and help,List Entity Types" permissions
    And I visit "/admin/structure/entity-types"
    Then I should not see the text "Add entity type"
    Given I am logged in as a user with the <permissions> permissions
    And I visit "/admin/structure/entity-type"
    And I click "Add entity type"
    Then I should get a "200" HTTP response

    Examples:
      | permissions                                                                       |
      | "Use the administration pages and help,List Entity Types,Create Entity Types"     |
      | "Use the administration pages and help,List Entity Types,Administer Entity Types" |

  @entity-type @drupal-perm
  Scenario Outline: Only allowed users can delete entity types
    Given I am logged in as a user with the "Use the administration pages and help,List Entity Types" permissions
    And I visit "/admin/structure/entity-types"
    Then I should not see the text "delete"
    Given I am logged in as a user with the <permissions> permissions
    And I visit "/admin/structure/entity-type"
    And I click "delete" in the "Vehicle" row
    Then I should get a "200" HTTP response

    Examples:
      | permissions                                                                       |
      | "Use the administration pages and help,List Entity Types,Delete Entity Types"     |
      | "Use the administration pages and help,List Entity Types,Administer Entity Types" |

  @cleanup
  Scenario Outline: This is a clean up step
    Given I am logged in as a user with the "Use the administration pages and help,Administer Entity Types,Administer Bundles,Administer Entities" permissions
    Given I visit "/admin/structure/entity-type"
    And I click <type_label>
    And I click "Delete"
    And I press the "Delete" button

    Examples:
      | type_label |
      | "Vehicle"  |
      | "Animal"   |
