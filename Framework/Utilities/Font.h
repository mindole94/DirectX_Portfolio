#pragma once

//class FontImporter
//{
//public:
//	FontImporter();
//	~FontImporter();
//
//	auto LoadFromFile(class Font* font, const std::string& path) -> const bool;
//
//private:
//	void ComputeTextureAtlasDimension(struct FT_FaceRec_* face, UINT& atlas_width, UINT& atlas_height, UINT& row_height);
//	auto GetCharacterMaxHeight(struct FT_FaceRec_* face) -> const int;
//	auto ErrorHandler(const int& error_code) -> const bool;
//
//private:
//	class Context* context = nullptr;
//	struct FT_LibraryRec_* ft_library = nullptr;
//};
//
//struct Glyph final
//{
//	int left;
//	int right;
//	int top;
//	int bottom;
//	int width;
//	int height;
//	float uv_left;
//	float uv_right;
//	float uv_top;
//	float uv_bottom;
//	int descent;
//	int horizontal_offset;
//};
//
//enum class HintingType : UINT
//{
//	None,
//	Light,
//	Normal,
//};
//
//class Font
//{
//public:
//	Font();
//	~Font();
//
//	const bool SaveToFile(const std::string& path);
//	const bool LoadFromFile(const std::string& path);
//
//	auto GetVertexBuffer() const { return vertex_buffer; }
//	auto GetIndexBuffer() const { return index_buffer; }
//	auto GetGlyphs() ->std::map<UINT, Glyph>& { return glyphs; }
//
//	auto GetHintingType() const -> const HintingType& { return hinting_type; }
//	auto IsForceAutoHint() const -> const bool& { return is_force_auto_hint; }
//
//	auto GetAtlas() const { return atlas; }
//	void SetAtlas(const std::shared_ptr<class TextureBuffer>& atlas) { this->atlas = atlas; }
//
//	auto GetFontColor() const -> const Color& { return font_color; }
//	void SetFontColor(const Color& color) { this->font_color = color; }
//
//	auto GetFontSize() const -> const UINT& { return font_size; }
//	void SetFontSize(const UINT& size);
//
//	void SetText(const std::string& text, const Vector2& position);
//
//private:
//	void UpdateBuffers();
//
//private:
//	std::shared_ptr<class ITexture> atlas;
//	std::shared_ptr<class VertexBuffer> vertex_buffer;
//	std::shared_ptr<class IndexBuffer> index_buffer;
//	std::vector<VertexTexture> vertices;
//	std::vector<UINT> indices;
//	std::map<UINT, Glyph> glyphs;
//
//	HintingType hinting_type = HintingType::Normal;
//	Color font_color = Color(1, 1, 1, 1);
//	std::string current_text = "";
//	UINT char_max_width = 0;
//	UINT char_max_height = 0;
//	UINT font_size = 16;
//	bool is_force_auto_hint = true;
//
//	FontImporter fontImporter;
//};