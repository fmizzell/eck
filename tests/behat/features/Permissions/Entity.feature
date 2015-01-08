@api @permissions
Feature: Entity Permissions
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

  @entity @drupal-perm
  Scenario: Users without the right permission can not access the entity's overview page
    Given I am logged in as a user with the "List Bundles" permissions
    And I visit "/admin/structure/entity-type/vehicle"
    Then I should see the text "Car"
    And I should not see the link "Car"

  @entity @drupal-perm
  Scenario Outline: Users with the right permission can access the entity's overview page (global)
    Given I am logged in as a user with the <permissions> permissions
    And I visit <path>
    Then I should see the link <link>
    When I click <link>
    Then I should get a "200" HTTP response

    Examples:
      | path                                   | link  | permissions                        |
      | "/admin/structure/entity-type/vehicle" | "Car" | "List Bundles,List Entities"       |
      | "/admin/structure/entity-type/animal"  | "Dog" | "List Bundles,List Entities"       |
      | "/admin/structure/entity-type/vehicle" | "Car" | "List Bundles,Administer Entities" |
      | "/admin/structure/entity-type/animal"  | "Dog" | "List Bundles,Administer Entities" |

  @entity @eck-perm
  Scenario Outline: Users with the right permission can access the entity's overview page (specific)
    Given I am logged in as a user with the "Use the administration pages and help,List Bundles" permissions
    And the last user has the ECK permission <operation> "entity" <object_id>
    And I visit "/admin/structure/entity-type/vehicle"
    Then I should see the link "Car"
    When I click "Car"
    Then I should get a "200" HTTP response

    Examples:
      | operation | object_id         |
      | "list"    | "vehicle\|car\|*" |
      | "*"       | "vehicle\|car\|*" |

  @entity @drupal-perm
  Scenario: Users without the right permission can not add entities
    Given I am logged in as a user with the "List Entities" permissions
    And I visit "/admin/structure/entity-type/vehicle/car"
    Then I should not see the link "Add Car"

  @entity @drupal-perm
  Scenario Outline: Users with the right permission can add entities (global)
    Given I am logged in as a user with the <permissions> permissions
    And I visit <path>
    Then I should see the link <link>
    When I click <link>
    Then I should get a "200" HTTP response

    Examples:
      | path                                       | link      | permissions                     |
      | "/admin/structure/entity-type/vehicle/car" | "Add Car" | "List Entities,Create Entities" |
      | "/admin/structure/entity-type/animal/dog"  | "Add Dog" | "List Entities,Create Entities" |
      | "/admin/structure/entity-type/vehicle/car" | "Add Car" | "Administer Entities"           |
      | "/admin/structure/entity-type/animal/dog"  | "Add Dog" | "Administer Entities"           |

  @entity @eck-perm
  Scenario Outline: Users with the right permission can add entities (specific)
    Given I am logged in as a user with the "Use the administration pages and help,List Entities" permissions
    And the last user has the ECK permission <operation> "entity" <object_id>
    And I visit "/admin/structure/entity-type/vehicle/car"
    Then I should see the link "Add Car"
    When I click "Add Car"
    Then I should get a "200" HTTP response

    Examples:
      | operation | object_id         |
      | "create"  | "vehicle\|car\|*" |
      | "*"       | "vehicle\|car\|*" |

  @entity @drupal-perm
  Scenario: Users without the right permission can not edit entities
    Given I am logged in as a user with the "List Entities" permissions
    And I visit "/admin/structure/entity-type/vehicle/car"
    Then I should not see the link "edit"

  @entity @drupal-perm
  Scenario Outline: Users with the right permission can edit entities (global)
    Given I am logged in as a user with the <permissions> permissions
    And I visit <path>
    Then I should see the link "edit"
    When I click "edit"
    Then I should get a "200" HTTP response

    Examples:
      | path                                       | permissions                     |
      | "/admin/structure/entity-type/vehicle/car" | "List Entities,Update Entities" |
      | "/admin/structure/entity-type/animal/dog"  | "List Entities,Update Entities" |
      | "/admin/structure/entity-type/vehicle/car" | "Administer Entities"           |
      | "/admin/structure/entity-type/animal/dog"  | "Administer Entities"           |

  @entity @eck-perm
  Scenario Outline: Users with the right permission can edit entities (specific)
    Given I am logged in as a user with the "Use the administration pages and help,List Entities" permissions
    And the last user has the ECK permission <operation> "entity" <object_id>
    And I visit "/admin/structure/entity-type/vehicle/car"
    Then I should see the link "edit"
    When I click "edit"
    Then I should get a "200" HTTP response

    Examples:
      | operation | object_id         |
      | "update"  | "vehicle\|car\|*" |
      | "*"       | "vehicle\|car\|*" |

 @entity @drupal-perm
  Scenario: Users without the right permission can not delete entities
    Given I am logged in as a user with the "List Entities" permissions
    And I visit "/admin/structure/entity-type/vehicle/car"
    Then I should not see the link "delete"

  @entity @drupal-perm
  Scenario Outline: Users with the right permission can delete entities (global)
    Given I am logged in as a user with the <permissions> permissions
    And I visit <path>
    Then I should see the link "delete"
    When I click "delete"
    Then I should get a "200" HTTP response

    Examples:
      | path                                       | permissions                     |
      | "/admin/structure/entity-type/vehicle/car" | "List Entities,Delete Entities" |
      | "/admin/structure/entity-type/animal/dog"  | "List Entities,Delete Entities" |
      | "/admin/structure/entity-type/vehicle/car" | "Administer Entities"           |
      | "/admin/structure/entity-type/animal/dog"  | "Administer Entities"           |

  @entity @eck-perm
  Scenario Outline: Users with the right permission can delete entities (specific)
    Given I am logged in as a user with the "Use the administration pages and help,List Entities" permissions
    And the last user has the ECK permission <operation> "entity" <object_id>
    And I visit "/admin/structure/entity-type/vehicle/car"
    Then I should see the link "delete"
    When I click "delete"
    Then I should get a "200" HTTP response

    Examples:
      | operation | object_id         |
      | "delete"  | "vehicle\|car\|*" |
      | "*"       | "vehicle\|car\|*" |


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
