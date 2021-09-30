#include "Framework.h"
#include "Font.h"

//#pragma warning(push, 0)
//#include <ft2build.h>
//#include <freetype/freetype.h>
//#pragma warning(pop)
//
//#define GLYPH_START     32
//#define GLYPH_END       127
//#define ATLAS_MAX_WIDTH 512
//
//static FT_UInt32 char_flags = FT_LOAD_DEFAULT | FT_LOAD_RENDER;
//
//FontImporter::FontImporter()
//{
//	if (FT_Init_FreeType(&ft_library))
//	{
//		std::cout << "Failed to initialized" << std::endl;
//		return;
//	}
//
//	FT_Int major, minor, revision;
//	FT_Library_Version(ft_library, &major, &minor, &revision);
//}
//
//FontImporter::~FontImporter()
//{
//	FT_Done_FreeType(ft_library);
//}
//
//auto FontImporter::LoadFromFile(Font * font, const std::string & path) -> const bool
//{
//	char_flags |= font->IsForceAutoHint() ? FT_LOAD_FORCE_AUTOHINT : 0;
//	switch (font->GetHintingType())
//	{
//	case HintingType::None:
//		char_flags |= FT_LOAD_NO_HINTING;
//		break;
//	case HintingType::Light:
//		char_flags |= FT_LOAD_TARGET_LIGHT;
//		break;
//	case HintingType::Normal:
//		char_flags |= FT_LOAD_TARGET_NORMAL;
//		break;
//	}
//
//	// Load font
//	FT_Face face;
//	if (ErrorHandler(FT_New_Face(ft_library, path.c_str(), 0, &face)))
//	{
//		FT_Done_Face(face);
//		return false;
//	}
//
//	// Set size
//	if (ErrorHandler(FT_Set_Char_Size(
//		face,					    // handle to face object
//		0,						    // char_width in 1/64th of points 
//		font->GetFontSize() * 64,	// char_height in 1/64th of points
//		96,						    // horizontal device resolution
//		96)))					    // vertical device resolution
//	{
//		FT_Done_Face(face);
//		return false;
//	}
//
//	// Font Atlas 크기 추정
//	unsigned int atlas_width = 0;
//	unsigned int atlas_height = 0;
//	unsigned int row_height = 0;
//	ComputeTextureAtlasDimension(face, atlas_width, atlas_height, row_height);
//	std::vector<BYTE> atlas_buffer;
//	atlas_buffer.resize(atlas_width * atlas_height);
//	atlas_buffer.reserve(atlas_buffer.size());
//
//	// 각 Glyph 확인
//	unsigned int pen_x = 0, pen_y = 0;
//	for (unsigned int i = GLYPH_START; i < GLYPH_END; i++)
//	{
//		// 문제가 있는 Glyph 건너 뛰기
//		if (ErrorHandler(FT_Load_Char(face, i, char_flags)))
//			continue;
//
//		FT_Bitmap* bitmap = &face->glyph->bitmap;
//
//		//Texture Atlas의 높이를 행에 맞게 증가
//		if (pen_x + bitmap->width >= atlas_width)
//		{
//			pen_x = 0;
//			pen_y += row_height;
//		}
//
//		for (UINT y = 0; y < bitmap->rows; y++)
//		{
//			for (UINT x = 0; x < bitmap->width; x++)
//			{
//				UINT _x = pen_x + x;
//				UINT _y = pen_y + y;
//				auto atlas_pos = _x + _y * atlas_width;
//				assert(atlas_buffer.size() > atlas_pos);
//
//				switch (bitmap->pixel_mode)
//				{
//				case FT_PIXEL_MODE_MONO: {
//					// TODO:
//				} break;
//
//				case FT_PIXEL_MODE_GRAY: {
//					atlas_buffer[atlas_pos] = static_cast<BYTE>(bitmap->buffer[x + y * bitmap->width]);
//				} break;
//
//				case FT_PIXEL_MODE_BGRA: {
//					// TODO:
//				} break;
//
//				default:
//					std::cout << "Font uses unsupported pixel format" << std::endl;
//					break;
//				}
//			}
//		}
//		atlas_buffer.shrink_to_fit();
//
//		Glyph glyph;
//		glyph.left = pen_x;
//		glyph.top = pen_y;
//		glyph.right = pen_x + bitmap->width;
//		glyph.bottom = pen_y + bitmap->rows;
//		glyph.width = glyph.right - glyph.left;
//		glyph.height = glyph.bottom - glyph.top;
//		glyph.uv_left = (float)glyph.left / (float)atlas_width;
//		glyph.uv_right = (float)glyph.right / (float)atlas_width;
//		glyph.uv_top = (float)glyph.top / (float)atlas_height;
//		glyph.uv_bottom = (float)glyph.bottom / (float)atlas_height;
//		glyph.descent = row_height - face->glyph->bitmap_top;
//		glyph.horizontal_offset = face->glyph->advance.x >> 6;
//
//
//		if (i >= 1 && FT_HAS_KERNING(face))
//		{
//			FT_Vector kerningVec;
//			FT_Get_Kerning(face, i - 1, i, FT_KERNING_DEFAULT, &kerningVec);
//			glyph.horizontal_offset += kerningVec.x >> 6;
//		}
//		glyph.horizontal_offset += face->glyph->metrics.horiBearingX;
//
//		font->GetGlyphs()[i] = glyph;
//
//		pen_x += bitmap->width + 1;
//	}
//
//	FT_Done_Face(face);
//
//	auto atlas = std::make_shared<TextureBuffer>(context, atlas_width, atlas_height, DXGI_FORMAT_R8_UNORM, atlas_buffer);
//	if (!atlas)
//	{
//		std::cout << "Failed to create shader resource." << std::endl;
//		return false;
//	}
//	font->SetAtlas(atlas);
//
//	return true;
//}
//
//void FontImporter::ComputeTextureAtlasDimension(FT_FaceRec_ * face, UINT & atlas_width, UINT & atlas_height, UINT & row_height)
//{
//	int penX = 0;
//	row_height = GetCharacterMaxHeight(face);
//
//	for (int i = GLYPH_START; i < GLYPH_END; i++)
//	{
//		if (ErrorHandler(FT_Load_Char(face, i, FT_LOAD_RENDER | FT_LOAD_TARGET_LIGHT)))
//			continue;
//
//		FT_Bitmap* bitmap = &face->glyph->bitmap;
//
//		penX += bitmap->width + 1;
//		//atlas_height = Math::mi(atlas_height, bitmap->rows);
//		atlas_height = atlas_height > bitmap->rows ? atlas_height : bitmap->rows;
//
//		if (penX + bitmap->width >= ATLAS_MAX_WIDTH)
//		{
//			penX = 0;
//			atlas_height += row_height;
//			atlas_width = ATLAS_MAX_WIDTH;
//		}
//	}
//}
//
//auto FontImporter::GetCharacterMaxHeight(FT_FaceRec_ * face) -> const int
//{
//	int max_height = 0;
//	for (UINT i = GLYPH_START; i < GLYPH_END; i++)
//	{
//		if (ErrorHandler(FT_Load_Char(face, i, FT_LOAD_RENDER)))
//			continue;
//
//		FT_Bitmap* bitmap = &face->glyph->bitmap;
//		//max_height = Math::Max<UINT>(max_height, bitmap->rows);
//		max_height = max_height > bitmap->rows ? max_height : bitmap->rows;
//	}
//
//	return max_height;
//}
//
//auto FontImporter::ErrorHandler(const int & error_code) -> const bool
//{
//	if (error_code == FT_Err_Ok)
//		return false;
//
//	switch (error_code)
//	{
//		// Generic errors
//	case FT_Err_Cannot_Open_Resource:	        std::cout << "Cannot open resource." << std::endl;                    break;
//	case FT_Err_Unknown_File_Format:	        std::cout<<"Unknown file format." << std::endl;                       break;
//	case FT_Err_Invalid_File_Format:	        std::cout<<"Broken file." << std::endl;                               break;
//	case FT_Err_Invalid_Version:		        std::cout<<"Invalid FreeType version." << std::endl;                  break;
//	case FT_Err_Lower_Module_Version:	        std::cout<<"Module version is too low." << std::endl;                 break;
//	case FT_Err_Invalid_Argument:		        std::cout<<"Invalid argument." << std::endl;                          break;
//	case FT_Err_Unimplemented_Feature:	        std::cout<<"Unimplemented feature." << std::endl;                     break;
//	case FT_Err_Invalid_Table:			        std::cout<<"Invalid table."<< std::endl;                              break;
//	case FT_Err_Invalid_Offset:			        std::cout<<"Invalid offset."<< std::endl;                             break;
//	case FT_Err_Array_Too_Large:		        std::cout<<"Array allocation size too large."<< std::endl;            break;
//	case FT_Err_Missing_Module:			        std::cout<<"Missing module."<< std::endl;                             break;
//	case FT_Err_Missing_Property:		        std::cout<<"Missing property."<< std::endl;                           break;
//
//		// Glyph/character errors
//	case FT_Err_Invalid_Glyph_Index:	        std::cout<<"Invalid glyph index."<< std::endl;                        break;
//	case FT_Err_Invalid_Character_Code:	        std::cout<<"Invalid character code."<< std::endl;                     break;
//	case FT_Err_Invalid_Glyph_Format:	        std::cout<<"Unsupported glyph format."<< std::endl;                   break;
//	case FT_Err_Cannot_Render_Glyph:	        std::cout<<"Cannot render this glyph format."<< std::endl;            break;
//	case FT_Err_Invalid_Outline:		        std::cout<<"Invalid outline."<< std::endl;                            break;
//	case FT_Err_Invalid_Composite:		        std::cout<<"Invalid composite glyph."<< std::endl;                    break;
//	case FT_Err_Too_Many_Hints:			        std::cout<<"Too many hints."<< std::endl;                             break;
//	case FT_Err_Invalid_Pixel_Size:		        std::cout<<"Invalid pixel size."<< std::endl;                         break;
//
//		// Handle errors
//	case FT_Err_Invalid_Handle:			        std::cout<<"Invalid object handle."<< std::endl;                      break;
//	case FT_Err_Invalid_Library_Handle:	        std::cout<<"Invalid library handle."<< std::endl;                     break;
//	case FT_Err_Invalid_Driver_Handle:	        std::cout<<"Invalid module handle."<< std::endl;                      break;
//	case FT_Err_Invalid_Face_Handle:	        std::cout<<"Invalid face handle."<< std::endl;                        break;
//	case FT_Err_Invalid_Size_Handle:	        std::cout<<"Invalid size handle."<< std::endl;                        break;
//	case FT_Err_Invalid_Slot_Handle:	        std::cout<<"Invalid glyph slot handle."<< std::endl;                  break;
//	case FT_Err_Invalid_CharMap_Handle:	        std::cout<<"Invalid charmap handle."<< std::endl;                     break;
//	case FT_Err_Invalid_Cache_Handle:	        std::cout<<"Invalid cache manager handle."<< std::endl;               break;
//	case FT_Err_Invalid_Stream_Handle:	        std::cout<<"Invalid stream handle."<< std::endl;                      break;
//
//		// Driver errors
//	case FT_Err_Too_Many_Drivers:		        std::cout<<"Too many modules."<< std::endl;                           break;
//	case FT_Err_Too_Many_Extensions:	        std::cout<<"Too many extensions."<< std::endl;                        break;
//
//		// Memory errors
//	case FT_Err_Out_Of_Memory:		            std::cout<<"Out of memory."<< std::endl;                              break;
//	case FT_Err_Unlisted_Object:	            std::cout<<"Unlisted object."<< std::endl;                            break;
//
//		// Stream errors
//	case FT_Err_Cannot_Open_Stream:			    std::cout<<"Cannot open stream."<< std::endl;                         break;
//	case FT_Err_Invalid_Stream_Seek:		    std::cout<<"Invalid stream seek."<< std::endl;                        break;
//	case FT_Err_Invalid_Stream_Skip:		    std::cout<<"Invalid stream skip."<< std::endl;                        break;
//	case FT_Err_Invalid_Stream_Read:		    std::cout<<"Invalid stream read."<< std::endl;                        break;
//	case FT_Err_Invalid_Stream_Operation:	    std::cout<<"Invalid stream operation."<< std::endl;                   break;
//	case FT_Err_Invalid_Frame_Operation:	    std::cout<<"Invalid frame operation."<< std::endl;                    break;
//	case FT_Err_Nested_Frame_Access:		    std::cout<<"Nested frame access."<< std::endl;                        break;
//	case FT_Err_Invalid_Frame_Read:			    std::cout<<"Invalid frame read."<< std::endl;                         break;
//
//		// Raster errors
//	case FT_Err_Raster_Uninitialized:	        std::cout<<"Raster uninitialized."<< std::endl;                       break;
//	case FT_Err_Raster_Corrupted:		        std::cout<<"Raster corrupted."<< std::endl;                           break;
//	case FT_Err_Raster_Overflow:		        std::cout<<"Raster overflow."<< std::endl;                            break;
//	case FT_Err_Raster_Negative_Height:	        std::cout<<"Negative height while rastering."<< std::endl;            break;
//
//		// Cache errors
//	case FT_Err_Too_Many_Caches:	            std::cout<<"Too many registered caches."<< std::endl;                 break;
//
//		// TrueType and SFNT errors 
//	case FT_Err_Invalid_Opcode:				    std::cout<<"Invalid opcode."<< std::endl;                             break;
//	case FT_Err_Too_Few_Arguments:			    std::cout<<"Too few arguments."<< std::endl;                          break;
//	case FT_Err_Stack_Overflow:				    std::cout<<"Stack overflow."<< std::endl;                             break;
//	case FT_Err_Code_Overflow:				    std::cout<<"Code overflow."<< std::endl;                              break;
//	case FT_Err_Bad_Argument:				    std::cout<<"Bad argument."<< std::endl;                               break;
//	case FT_Err_Divide_By_Zero:				    std::cout<<"Division by zero."<< std::endl;                           break;
//	case FT_Err_Invalid_Reference:			    std::cout<<"Invalid reference."<< std::endl;                          break;
//	case FT_Err_Debug_OpCode:				    std::cout<<"Found debug opcode."<< std::endl;                         break;
//	case FT_Err_ENDF_In_Exec_Stream:		    std::cout<<"Found ENDF opcode in execution stream."<< std::endl;      break;
//	case FT_Err_Nested_DEFS:				    std::cout<<"Nested DEFS."<< std::endl;                                break;
//	case FT_Err_Invalid_CodeRange:			    std::cout<<"Invalid code range."<< std::endl;                         break;
//	case FT_Err_Execution_Too_Long:			    std::cout<<"Execution context too long."<< std::endl;                 break;
//	case FT_Err_Too_Many_Function_Defs:		    std::cout<<"Too many function definitions."<< std::endl;              break;
//	case FT_Err_Too_Many_Instruction_Defs:	    std::cout<<"Too many instruction definitions."<< std::endl;           break;
//	case FT_Err_Table_Missing:				    std::cout<<"SFNT font table missing."<< std::endl;                    break;
//	case FT_Err_Horiz_Header_Missing:		    std::cout<<"Horizontal header (hhea) table missing."<< std::endl;     break;
//	case FT_Err_Locations_Missing:			    std::cout<<"Locations (loca) table missing."<< std::endl;             break;
//	case FT_Err_Name_Table_Missing:			    std::cout<<"Name table missing."<< std::endl;                         break;
//	case FT_Err_CMap_Table_Missing:			    std::cout<<"Character map (cmap) table missing."<< std::endl;         break;
//	case FT_Err_Hmtx_Table_Missing:			    std::cout<<"Horizontal metrics (hmtx) table missing."<< std::endl;    break;
//	case FT_Err_Post_Table_Missing:			    std::cout<<"PostScript (post) table missing."<< std::endl;            break;
//	case FT_Err_Invalid_Horiz_Metrics:		    std::cout<<"Invalid horizontal metrics."<< std::endl;                 break;
//	case FT_Err_Invalid_CharMap_Format:		    std::cout<<"Invalid character map (cma) format."<< std::endl;         break;
//	case FT_Err_Invalid_PPem:				    std::cout<<"Invalid ppem value."<< std::endl;                         break;
//	case FT_Err_Invalid_Vert_Metrics:		    std::cout<<"Invalid vertical metrics."<< std::endl;                   break;
//	case FT_Err_Could_Not_Find_Context:		    std::cout<<"Could not find context."<< std::endl;                     break;
//	case FT_Err_Invalid_Post_Table_Format:	    std::cout<<"Invalid PostScript (post) table format."<< std::endl;     break;
//	case FT_Err_Invalid_Post_Table:			    std::cout<<"Invalid PostScript (post) table."<< std::endl;            break;
//	case FT_Err_DEF_In_Glyf_Bytecode:		    std::cout<<"Found FDEF or IDEF opcode in glyf bytecode."<< std::endl; break;
//
//		// CFF, CID, and Type 1 errors 
//	case FT_Err_Syntax_Error:			        std::cout<<"Opcode syntax error."<< std::endl;                        break;
//	case FT_Err_Stack_Underflow:		        std::cout<<"Argument stack underflow."<< std::endl;                   break;
//	case FT_Err_Ignore:					        std::cout<<"Ignore."<< std::endl;                                     break;
//	case FT_Err_No_Unicode_Glyph_Name:	        std::cout<<"No Unicode glyph name found."<< std::endl;                break;
//	case FT_Err_Glyph_Too_Big:			        std::cout<<"Glyph too big for hinting."<< std::endl;                  break;
//
//		// BDF errors
//	case FT_Err_Missing_Startfont_Field:		std::cout<<"'STARTFONT' field missing."<< std::endl;                  break;
//	case FT_Err_Missing_Font_Field:				std::cout<<"'FONT' field missing."<< std::endl;                       break;
//	case FT_Err_Missing_Size_Field:				std::cout<<"'SIZE' field missing."<< std::endl;                       break;
//	case FT_Err_Missing_Fontboundingbox_Field:	std::cout<<"'FONTBOUNDINGBOX' field missing."<< std::endl;            break;
//	case FT_Err_Missing_Chars_Field:			std::cout<<"'CHARS' field missing."<< std::endl;                      break;
//	case FT_Err_Missing_Startchar_Field:		std::cout<<"'STARTCHAR' field missing."<< std::endl;                  break;
//	case FT_Err_Missing_Encoding_Field:			std::cout<<"'ENCODING' field missing."<< std::endl;                   break;
//	case FT_Err_Missing_Bbx_Field:				std::cout<<"'BBX' field missing."<< std::endl;                        break;
//	case FT_Err_Bbx_Too_Big:					std::cout<<"'BBX' too big."<< std::endl;                              break;
//	case FT_Err_Corrupted_Font_Header:			std::cout<<"Font header corrupted or missing fields."<< std::endl;    break;
//	case FT_Err_Corrupted_Font_Glyphs:			std::cout<<"Font glyphs corrupted or missing fields."<< std::endl;    break;
//
//		// None
//	default:                                    std::cout<<"Unknown error code."<< std::endl;                         break;
//	}
//
//	return true;
//}
//
////////////////////////////////////////////////////////
//
//#define ASCII_TAB       9
//#define ASCII_NEW_LINE  10
//#define ASCII_SPACE     32
//
//Font::Font()
//{
//	vertex_buffer = std::make_shared<VertexBuffer>();
//	index_buffer = std::make_shared<IndexBuffer>();
//}
//
//Font::~Font()
//{
//}
//
//const bool Font::SaveToFile(const std::string & path)
//{
//	return true;
//}
//
//const bool Font::LoadFromFile(const std::string & path)
//{
//	if (fontImporter.LoadFromFile(this, path))
//	{
//		std::cout << "Failed to load font " << path.c_str() << std::endl;
//		return false;
//	}
//
//	for (const auto& char_info : glyphs)
//	{
//		//char_max_width = Math::Max<int>(char_info.second.width, char_max_width);
//		//char_max_height = Math::Max<int>(char_info.second.height, char_max_height);
//		char_max_width = (int)(char_info.second.width > char_max_width ? char_info.second.width : char_max_width);
//		char_max_height = (int)(char_info.second.height, char_max_height ? char_info.second.height : char_max_height);
//	}
//
//	return true;
//}
//
//void Font::SetFontSize(const UINT & size)
//{
//	font_size = Math::Clamp(size, 8U, 50U);
//}
//
//void Font::SetText(const std::string & text, const Vector2 & position)
//{
//	if (text == current_text)
//		return;
//
//	auto pen = position;
//	current_text = text;
//
//	vertices.clear();
//	indices.clear();
//
//	for (const auto& text_char : current_text)
//	{
//		auto glyph = glyphs[text_char];
//
//		if (text_char == ASCII_TAB)
//		{
//			int space_offset = glyphs[ASCII_SPACE].horizontal_offset;
//			int space_count = 8;
//			int tab_spacing = space_offset * space_count;
//			int column_header = int(pen.x - position.x);
//			int offset_to_next_tab_stop = tab_spacing - (column_header % (tab_spacing != 0 ? tab_spacing : 1));
//			pen.x += offset_to_next_tab_stop;
//			continue;
//		}
//
//		if (text_char == ASCII_NEW_LINE)
//		{
//			pen.y = pen.y - char_max_height;
//			pen.x = position.x;
//			continue;
//		}
//
//		if (text_char == ASCII_SPACE)
//		{
//			pen.x += glyph.horizontal_offset;
//			continue;
//		}
//
//		vertices.emplace_back(Vector3(pen.x, (pen.y - glyph.height - glyph.descent), 0.0f), Vector2(glyph.uv_left, glyph.uv_bottom));	// Bottom left
//		vertices.emplace_back(Vector3(pen.x, pen.y - glyph.descent, 0.0f), Vector2(glyph.uv_left, glyph.uv_top));		// Top left
//		vertices.emplace_back(Vector3((pen.x + glyph.width), (pen.y - glyph.height - glyph.descent), 0.0f), Vector2(glyph.uv_right, glyph.uv_bottom));	// Bottom right
//		vertices.emplace_back(Vector3((pen.x + glyph.width), (pen.y - glyph.height - glyph.descent), 0.0f), Vector2(glyph.uv_right, glyph.uv_bottom));	// Bottom right
//		vertices.emplace_back(Vector3(pen.x, pen.y - glyph.descent, 0.0f), Vector2(glyph.uv_left, glyph.uv_top));		// Top left
//		vertices.emplace_back(Vector3((pen.x + glyph.width), pen.y - glyph.descent, 0.0f), Vector2(glyph.uv_right, glyph.uv_top));	    // Top right
//
//		pen.x = pen.x + glyph.width;
//	}
//
//	for (UINT i = 0; i < vertices.size(); i++)
//		indices.emplace_back(i);
//
//	UpdateBuffers();
//}
//
//void Font::UpdateBuffers()
//{
//	if (!vertex_buffer || !index_buffer)
//	{
//		std::cout << "Invaild parameter" << std::endl;
//		return;
//	}
//
//	if (vertices.size() > vertex_buffer->Count())
//	{
//		vertex_buffer->Clear();
//
//		vertex_buffer->Create(vertices, D3D11_USAGE_DYNAMIC);
//		//vertex_buffer = std::make_shared<VertexBuffer>();
//		//index_buffer = std::make_shared<IndexBuffer>();
//		vertex_buffer = new VertexBuffer(vertices, vertices.size() * 4, sizeof(VertexTexture));
//		index_buffer->Clear();
//		index_buffer->Create(indices, D3D11_USAGE_DYNAMIC);
//	}
//
//	auto vertex_data = static_cast<VertexTexture*>(vertex_buffer->Map());
//	std::copy(vertices.begin(), vertices.end(), vertex_data);
//	vertex_buffer->Unmap();
//
//	auto index_data = static_cast<UINT*>(index_buffer->Map());
//	std::copy(indices.begin(), indices.end(), index_data);
//	index_buffer->Unmap();
//}
//
