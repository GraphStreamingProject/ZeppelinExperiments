#pragma once

#include <cstdint>
#include <tuple>
#include <vector>
#include <iostream>
#include <fstream>
#include <mutex>
#include <condition_variable>
#include <thread>

#include "graph_stream.h"

class BufferedEdgeInput
{
 public:
  BufferedEdgeInput(const std::string &path, size_t buffer_size, bool insert_delete_tag = true) :
    num_nodes(),
    num_edges(),
    mutex(),
    writeBufferSignal(),
    buffers(),
    readBuffer(&buffers[0]),
    writeBuffer(&buffers[1]),
    BUFFER_SIZE(buffer_size),
    currentElement(buffer_size),
    stream(path.c_str(), 32 * 1024),
    swapWaiting(false),
    endOfFile(false),
    insert_delete_tag(insert_delete_tag),
    reader()
  {
    num_nodes = stream.nodes;
    num_edges = stream.edges;
    reader = std::thread(&BufferedEdgeInput::ingest, this);
    readBuffer->reserve(buffer_size);
    writeBuffer->reserve(buffer_size);
  }

  ~BufferedEdgeInput()
  {
    reader.join();
  }

  bool get_edge(std::tuple<uint32_t, uint32_t, bool> &out)
  {
    if (currentElement >= readBuffer->size())
    {
      readBuffer->clear();
      {
        std::unique_lock<std::mutex> lk(mutex);
        writeBufferSignal.wait(lk, [&](){return swapWaiting || endOfFile;});
        std::swap(writeBuffer, readBuffer);
        currentElement = 0;
        swapWaiting = false;
      }
      writeBufferSignal.notify_all();
    }
    
    if (currentElement < readBuffer->size())
    {
      out = (*readBuffer)[currentElement++];
      return true;
    }
    
    return false;
  }

  void ingest()
  {
    while (num_ingest < num_edges)
    {
      for (size_t i = 0; i < BUFFER_SIZE && num_ingest < num_edges; ++i)
      {
        uint32_t src_vertex;
	uint32_t dst_vertex;
	uint8_t tag;
	stream.parse_line(&tag, &src_vertex, &dst_vertex);

        writeBuffer->emplace_back(src_vertex, dst_vertex, tag);
	num_ingest++;
      }

      {
        std::unique_lock<std::mutex> lk(mutex);
	swapWaiting = true;
	writeBufferSignal.notify_all();
        writeBufferSignal.wait(lk, [&](){return !swapWaiting;});
      }
    }
    std::unique_lock<std::mutex> lk(mutex);
    swapWaiting = true;
    endOfFile = true;
    writeBufferSignal.notify_all();
    return;
  }

  size_t num_nodes;
  size_t num_edges;
  
 private:
  std::mutex mutex;
  std::condition_variable writeBufferSignal;
  std::vector<std::tuple<uint32_t, uint32_t, bool>> buffers[2];
  std::vector<std::tuple<uint32_t, uint32_t, bool>>* readBuffer;
  std::vector<std::tuple<uint32_t, uint32_t, bool>>* writeBuffer;
  const size_t BUFFER_SIZE;
  size_t currentElement;
  GraphStream stream;
  bool swapWaiting;
  bool endOfFile;
  bool insert_delete_tag;
  std::thread reader;
  uint64_t num_ingest = 0;
};
