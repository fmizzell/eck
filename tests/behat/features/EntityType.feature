@api
Feature: Entity type management
  As a content architect
  I want to be able to create custom entity types
  so my content will be as lean and specific as possible.
  
  Background:
    Given I am logged in as a user with the "administrator" role
  
  Scenario: The entity type management page is completely functional
    Given I visit "/admin/structure/entity-type"
    Then I should see the heading "Entity types"
    Then I should see the link "Add entity type"
 
  @entity-type-create
  Scenario: I am able to create entity types
    Given I visit "/admin/structure/entity-type"
    And I click "Add entity type"
    And I fill in "Entity Type" with "Test 1258767899"
    And I fill in "Machine-readable name" with "test"
    And I press the "Save" button
    Then I should see the text "Entity type Test 1258767899 has been updated."
    
    Given I visit "/admin/structure/entity-type"
    Then I should see the text "Test 1258767899"

  @entity-type-delete
  Scenario: I am able to create entity types
    Given I visit "/admin/structure/entity-type"
    And I click "Test 1258767899"
    And I click "Delete"
    And I press the "Delete" button
    Then I should see the text "Entity type 'test' has been deleted"
    And I should not see "Test 1258767899"
    
