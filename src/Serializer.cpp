#include "Serializer.h"
#include <iomanip>
#include <iostream>

typedef double double_t;

// For correcting endianness issues; you may not need these.
inline uint8_t Reverse(uint8_t value) {
	value = (value & 0xF0) >> 4 | (value & 0x0F) << 4;
	value = (value & 0xCC) >> 2 | (value & 0x33) << 2;
	value = (value & 0xAA) >> 1 | (value & 0x55) << 1;
	return value;
}

// For correcting endianness issues; you may not need these.
inline uint32_t Reverse(uint32_t value) {
	return (value & 0xFF000000) >> 24 |
		(value & 0x00FF0000) >> 8 |
		(value & 0x0000FF00) << 8 |
		(value & 0x000000FF) << 24;
}

inline uint64_t Reverse(uint64_t value) {
    return (value & 0xFF00000000000000) >> 56 |
        (value & 0x00FF000000000000) >> 40 |
        (value & 0x0000FF0000000000) >> 24 |
        (value & 0x000000FF00000000) >> 8 |
        (value & 0x00000000FF000000) << 8 |
        (value & 0x0000000000FF0000) << 24 |
        (value & 0x000000000000FF00) << 40 |
        (value & 0x00000000000000FF) << 56;
}

inline double_t Reverse(double_t value) {
	auto result = Reverse(*reinterpret_cast<uint32_t*>(&value));
	return *reinterpret_cast<double_t*>(&result);
}

// at this point, "index" contains the in-memory inverted index 
// now we save the index to disk, building three files: the postings index,
// the vocabulary list, and the vocabulary table.
void Serializer::buildIndex(const boost::filesystem::path &filePath, const InvertedIndex &auxIdx, 
    const std::unordered_map<uint32_t, std::string> &idTable, const  std::vector<double_t> &weights) {
	// do i really need an array of terms? why not just iterate the hashmap with an advanced for loop?
    std::vector<uint32_t> vocabPositions = Serializer::buildVocab(filePath, auxIdx.getVocabList());
	Serializer::buildPostings(filePath, auxIdx, vocabPositions); // build and write postings to disk

    Serializer::buildEucDist(filePath, weights); // build and write document length to disk

	Serializer::buildIdTable(filePath, idTable);

    std::vector<uint32_t> authorPositions = Serializer::buildAuthors(filePath, auxIdx.getAuthorList());
    buildAuthorDocs(filePath, auxIdx, authorPositions);

	// std::vector<uint32_t> &gramsVocab = Serializer::buildGramVocab(gram1, gram2, ...); // FOR CHERIE (merge then sort list before passing into this function?)
	// Serializer::buildGramdex(filePath, gramsVocab, gram1, gram2, ...); // FOR CHERIE
}

void Serializer::buildPostings(const boost::filesystem::path &filePath, const InvertedIndex &auxIdx,
	const std::vector<uint32_t> &vocabPositions) {

	boost::filesystem::path postingsPath = filePath, tablePath = filePath;

	// simultaneously build the vocabulary table on disk, mapping a term index to a 
	// file location in the postings file.
    std::ofstream postingsFile(postingsPath.append("/postings.bin", boost::filesystem::path::codecvt()).string(),
		std::ios::out | std::ios::binary);
    std::ofstream vocabTable(tablePath.append("/vocabTable.bin", boost::filesystem::path::codecvt()).string(),
		std::ios::out | std::ios::binary);

    std::cout << "Writing to: " << postingsPath << std::endl;
    std::cout << "Writing to: " << tablePath << std::endl;

	// the first thing we must write to the vocabTable file is the number of vocab terms.
	uint32_t termCount = Reverse(auxIdx.getVocabList().size()); // UNCOMMENT FOR WINDOWS

	vocabTable.write((const char*)&termCount, sizeof(termCount));

	uint32_t vocabIndex = 0; // FOR EVERY TERM
	for (const std::string &term : auxIdx.getVocabList()) {
		const std::list<DocInfo> &postings = auxIdx.getPostings(term);

		// write the vocab table entry for this term: the byte location of the term in the vocab list file,
		// and the byte location of the postings for the term in the postings file.
		uint32_t vPosition = Reverse(vocabPositions[vocabIndex]);
		vocabTable.write((const char*)&vPosition, sizeof(vPosition));

		uint32_t pPosition = Reverse((uint32_t)postingsFile.tellp());
		vocabTable.write((const char*)&pPosition, sizeof(pPosition));
		
        writePostings(postingsFile, postings);
		
		vocabIndex++;
	}


	vocabTable.close();
	postingsFile.close();
}

/**
  * Writes a single list of postings for one vocabulary term to the given postings file.
  * Currently writes only the document frequency (size of list) and each document ID as a gap.
  */


// Writes a single list of postings for one vocabulary term to the given postings file.
// Currently writes only the document frequency (size of list) and each document ID as a gap.
void Serializer::writePostings(std::ofstream &postingsFile, const std::list<DocInfo> &postings) {
	// Write the document frequency.
	uint32_t docFreq = Reverse((uint32_t)postings.size());
	postingsFile.write((const char*)&docFreq, sizeof(docFreq));

	uint32_t lastDocId = 0;
	for (const DocInfo &currDoc : postings) {
		// write document ID gap.
		// Uses Reverse to fix endianness issues on Windows. If not building on Windows, you may
		// want to remove the Reverse calls.
		uint32_t docIdGap = Reverse((uint32_t)currDoc.getDocId() - (uint32_t)lastDocId);
		postingsFile.write((const char*)&docIdGap, sizeof(docIdGap)); // variable byte encode this

		uint32_t positionSize = Reverse((uint32_t)currDoc.getPositions().size());
		postingsFile.write((const char*)&positionSize, sizeof(positionSize)); // write to disk size of position list

		uint32_t lastPosGap = 0;
		for (const uint32_t &pos : currDoc.getPositions()) {
			uint32_t posGap = Reverse(pos - lastPosGap);
			postingsFile.write((const char*)&posGap, sizeof(posGap));

            lastPosGap = pos;
		}

		lastDocId = (uint32_t)currDoc.getDocId();
	}
}


std::vector<uint32_t> Serializer::buildVocab(const boost::filesystem::path &filePath, const std::set<std::string> &vocabList) {
	// first build the vocabulary list: a file of each vocab word concatenated together.
	// also build an array associating each term with its byte location in this file.
    std::vector<uint32_t> positions(vocabList.size());
	int vocabIndex = 0;

	boost::filesystem::path vocabPath = filePath;
    std::ofstream vocabFile(vocabPath.append("/vocabList.bin", boost::filesystem::path::codecvt()).string(),
		std::ios::out | std::ios::binary);

    std::cout << "Writing to: " << vocabPath << std::endl;

    for (const std::string &term : vocabList) {
		positions[vocabIndex] = vocabFile.tellp();
		vocabFile.write(term.c_str(), (sizeof(char) * term.length()));
		vocabIndex++;
	}

	vocabFile.close();
	return positions;
}

void Serializer::buildEucDist(const boost::filesystem::path &filePath, const std::vector<double_t> &weights) {
	boost::filesystem::path weightbPath = filePath;
	std::ofstream weightFile(weightbPath.append("/docWeights.bin", boost::filesystem::path::codecvt()).string(),
		std::ios::out | std::ios::binary);

    uint32_t size = Reverse((uint32_t)weights.size());
	weightFile.write((const char*)&size, sizeof(size)); // write to disk length of document to file

	for (const double_t &dist : weights) {
		//double_t encoded = Reverse(dist);
		double_t encoded = dist;
		weightFile.write((const char*)&encoded, sizeof(encoded)); // write to disk length of document to file
	}

	weightFile.close();
}

void Serializer::buildIdTable(const boost::filesystem::path &filePath, const std::unordered_map<uint32_t, std::string>& idTable)
{
	boost::filesystem::path weightbPath = filePath;
	std::ofstream idTableFile(weightbPath.append("/idTable.bin", boost::filesystem::path::codecvt()).string(),
		std::ios::out | std::ios::binary);

	uint32_t i, size = Reverse((uint32_t)idTable.size());
	//uint32_t i, size = (uint32_t)idTable.size(); // 
	idTableFile.write((const char*)&size, sizeof(size)); // write to disk length of document to file


	for (std::pair<uint32_t, std::string> pr : idTable) {
		uint32_t id = Reverse((uint32_t) pr.first);
		idTableFile.write((const char *) &id, sizeof(id)); // write doc id to disk

		uint32_t len = Reverse((uint32_t)pr.second.size());
		idTableFile.write((const char *) &len, sizeof(len)); // write size of string to disk

		for (i = 0; i < pr.second.size(); ++i) {
			//uint8_t ch = Reverse((uint8_t)pr.second[i]);
			uint8_t ch = (uint8_t)pr.second[i];

			idTableFile.write((const char*)&ch, sizeof(ch)); // write char values, string, to disk
		}
	}

	idTableFile.close();
}

std::vector<uint32_t> Serializer::buildAuthors(const boost::filesystem::path &filePath, const std::set<std::string> &authorList) {
    // first build the vocabulary list: a file of each vocab word concatenated together.
    // also build an array associating each term with its byte location in this file.
    std::vector<uint32_t> positions(authorList.size());
    int authorIndex = 0;

    boost::filesystem::path authorPath = filePath;
    std::ofstream authorFile(authorPath.append("/authorList.bin", boost::filesystem::path::codecvt()).string(),
        std::ios::out | std::ios::binary);

    std::cout << "Writing to: " << authorPath << std::endl;

    for (const std::string &term : authorList) {
        positions[authorIndex] = authorFile.tellp();
        authorFile.write(term.c_str(), (sizeof(char) * term.length()));
        authorIndex++;
    }

    authorFile.close();
    return positions;
}

void Serializer::buildAuthorDocs(const boost::filesystem::path &filePath, const InvertedIndex &auxIdx,
    const std::vector<uint32_t> &authorPositions) {

    boost::filesystem::path authorDocsPath = filePath, tablePath = filePath;

    // simultaneously build the vocabulary table on disk, mapping a term index to a
    // file location in the postings file.
    std::ofstream authorDocsFile(authorDocsPath.append("/authorDocs.bin", boost::filesystem::path::codecvt()).string(),
        std::ios::out | std::ios::binary);
    std::ofstream authorTable(tablePath.append("/authorTable.bin", boost::filesystem::path::codecvt()).string(),
        std::ios::out | std::ios::binary);

    std::cout << "Writing to: " << authorDocsPath << std::endl;
    std::cout << "Writing to: " << tablePath << std::endl;

    // the first thing we must write to the vocabTable file is the number of vocab terms.
    uint32_t termCount = Reverse(auxIdx.getAuthorList().size()); // UNCOMMENT FOR WINDOWS

    authorTable.write((const char*)&termCount, sizeof(termCount));

    uint32_t authorIndex = 0; // FOR EVERY TERM
    for (const std::string &author : auxIdx.getAuthorList()) {
        const std::list<uint32_t> authorDocs = auxIdx.getAuthorDocs(author);

        // write the vocab table entry for this term: the byte location of the term in the vocab list file,
        // and the byte location of the postings for the term in the postings file.
        uint32_t vPosition = Reverse(authorPositions[authorIndex]);
        authorTable.write((const char*)&vPosition, sizeof(vPosition));

        uint32_t pPosition = Reverse((uint32_t)authorDocsFile.tellp());
        authorTable.write((const char*)&pPosition, sizeof(pPosition));

        writeAuthorPostings(authorDocsFile, authorDocs);

        authorIndex++;
    }


    authorTable.close();
    authorDocsFile.close();
}

void Serializer::writeAuthorPostings(std::ofstream &postingsFile, const std::list<uint32_t> &postings) {
    // Write the document frequency.
    uint32_t docFreq = Reverse((uint32_t)postings.size());
    postingsFile.write((const char*)&docFreq, sizeof(docFreq));

    for (const uint32_t &docId : postings) {
        uint32_t revId = Reverse(docId);
        postingsFile.write((const char*)&revId, sizeof(revId));
    }

}
