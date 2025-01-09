#include"CreateBulkItemDSfromCSV.hxx"

int ITK_user_main(int 	argc, char* argv[]) {
	printf("Utility started...\n");

	tag_t type = NULLTAG;
	tag_t create_input = NULLTAG;
	tag_t NewItem = NULLTAG;
	tag_t userTag = NULLTAG;
	tag_t Ufolder = NULLTAG;
	tag_t uom = NULLTAG;
	tag_t DStype = NULLTAG;
	tag_t DScreate_input = NULLTAG;
	tag_t NewDS = NULLTAG;
	tag_t NewFolder = NULLTAG;
	tag_t Item_rev = NULLTAG;
	tag_t Huser_folder = NULLTAG;

	// docx tags 
	tag_t DocxDStype = NULLTAG;
	tag_t DocxDScreate_input = NULLTAG;
	tag_t DocxNewDS = NULLTAG;

	//GRM Relation creation--PDF
	tag_t relation_type = NULLTAG;
	tag_t GRMrelation = NULLTAG;


	// GRM relation creation--docx
	tag_t Docxrelation_type = NULLTAG;
	tag_t GRMrelationDocx = NULLTAG;

	// variable
	char* item_id = NULL;
	char* item_name = NULL;
	int status = ITK_ok;

	char* userID = ITK_ask_cli_argument("-u=");
	char* password = ITK_ask_cli_argument("-p=");
	char* group = ITK_ask_cli_argument("-g=");

	status = ITK_init_module(userID, password, group);
	if (status == ITK_ok) {
		printf("Login Successful.\n");
	}
	else {
		printf("Failed to login.\n");
	}

	status = TCTYPE_find_type("Folder", "Folder", &type);
	status = TCTYPE_construct_create_input(type, &create_input);
	status = AOM_set_value_string(create_input, "object_name", "Hawkins C frame");
	status = TCTYPE_create_object(create_input, &NewFolder);
	status = AOM_save_with_extensions(NewFolder);

	//find user Home folder	
	status = SA_find_user2(userID, &userTag);
	status = SA_ask_user_home_folder(userTag, &Huser_folder);
	status = FL_insert(Huser_folder, NewFolder, 999);
	status = AOM_save_with_extensions(Huser_folder);

	
	FILE* fp;
	char fline[500];
	char* ItemName;
	char* MakeBuy;
	char* DSpath;
	char* UOM;
	char* owner;
	char* DocxDSPath;


	fp = fopen("G:\\My Drive\\FaithPLM\\Online Batch Recorded Sessions\\4. Customization\\ITK_Program Practice\\Batch_Utility\\20-30 KVA 300-350 C FRAME HAWKINS COOKER\\20-30 KVA 300-350 C FRAME HAWKINS COOKER BOM.csv", "a+");
	printf("\nItem ID\t\tItem Name\tUOM\n");
	while (fgets(fline, 500, fp)) {
		ItemName = strtok(fline, ",");
		MakeBuy = strtok(NULL, ",");
		UOM = strtok(NULL, ",");
		DSpath = strtok(NULL, ",");
		DocxDSPath = strtok(NULL, ",");
		owner = strtok(NULL, "\N");

		//Item create
		status = TCTYPE_ask_type("A3BHMakePart", &type);
		status = TCTYPE_construct_create_input(type, &create_input);
		status = AOM_set_value_string(create_input, "object_name", ItemName);
		status = AOM_set_value_string(create_input, "uom_tag", UOM);
		status = AOM_set_value_string(create_input, "a3MakeBuy", MakeBuy);
		status = TCTYPE_create_object(create_input, &NewItem);
		status = AOM_save_with_extensions(NewItem);

		//get latest Rev
		status = ITEM_ask_latest_rev(NewItem, &Item_rev);

		//Dataset create PDF
		status = TCTYPE_find_type("PDF", "Dataset", &DStype);
		status = TCTYPE_construct_create_input(DStype, &DScreate_input);
		status = AOM_set_value_string(DScreate_input, "object_name", ItemName);
		status = TCTYPE_create_object(DScreate_input, &NewDS);
		status = AOM_save_with_extensions(NewDS);			
		
		//Dataset create DOCX
		status = TCTYPE_find_type("MSWordX", "Dataset", &DocxDStype);
		status = TCTYPE_construct_create_input(DocxDStype, &DocxDScreate_input);
		status = AOM_set_value_string(DocxDScreate_input, "object_name", ItemName);
		status = TCTYPE_create_object(DocxDScreate_input, &DocxNewDS);
		status = AOM_save_with_extensions(DocxNewDS);		

		// GRM relation --pdf
		status = GRM_find_relation_type("IMAN_specification", &relation_type);
		status = GRM_create_relation(Item_rev, NewDS, relation_type, NULLTAG, &GRMrelation);
		status = GRM_save_relation(GRMrelation);
		status = AOM_save_with_extensions(NewDS);
		status = AOM_save_with_extensions(NewItem);			

		// GRM relation --docx
		status = GRM_find_relation_type("IMAN_reference", &Docxrelation_type);
		status = GRM_create_relation(Item_rev, DocxNewDS, Docxrelation_type, NULLTAG, &GRMrelationDocx);
		status = GRM_save_relation(GRMrelationDocx);
		status = AOM_save_with_extensions(DocxNewDS);
		status = AOM_save_with_extensions(NewItem);

		//import name reference pdf and docx
		status = AOM_refresh(NewDS, TRUE); // CHECKOUT THE PDF DATASET	
		status = AOM_refresh(DocxNewDS, TRUE); // CHECKOUT THE WORD DATASET	
		status = AE_import_named_ref(NewDS, "PDF_Reference", DSpath, "", SS_BINARY); // giving double backward slash in must(\\), if not given file will not import in dataset
		status = AE_import_named_ref(DocxNewDS, "word", DocxDSPath, "", SS_BINARY);
		status = AOM_save_without_extensions(NewDS);
		status = AOM_save_without_extensions(DocxNewDS);
		status = AOM_refresh(NewDS, FALSE); // CHECKIN THE DATASET				
		status = AOM_refresh(DocxNewDS, FALSE); // CHECKin THE PDF DATASET


		// place item to home folder		
		status = FL_insert(NewFolder, NewItem, 999);
		status = AOM_save_with_extensions(NewFolder);
				 

		// printing newly created item	
		char* UOMvalue = NULL;
		status = ITEM_ask_id2(NewItem, &item_id);
		status = ITEM_ask_name2(NewItem, &item_name);
		status = ITEM_ask_unit_of_measure(NewItem, &uom);
		
		printf("%s\t%s\t%d\n", item_id, item_name, uom);

		MEM_free(item_id);
		MEM_free(item_name);

	}
	
	fclose(fp);

	status = ITK_exit_module(true);
	if (status == ITK_ok) {
		printf("\n\nLogout Successful.\n");
	}
	else {
		printf("\nFailed to logout.\n");
	}

	
	printf("Utlity completed.\n");	

	(void)_getch();
	return status;
}